// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackState.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"


//SPONGE: SHOULD ADD OBJ MEMBER ACCESS VALIDATIONS
bool UAttackState::StateValidation_Implementation() {
	return this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->StatesComp->CompContext->CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void UAttackState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{	
	bool IsStaminaAllowed = this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->StatesComp->CompContext->CharacterActor,EAttributeCode::ATT_Stamina) > 0 : true;

	if (Action == EActionList::ActionAttack && EventType == IE_Pressed && IsStaminaAllowed && this->StatesComp->CrossStateData.IsComboActive) {
		this->StartAttack();
	}

	if (Action == EActionList::ActionAttack && EventType == IE_Released && IsStaminaAllowed && this->StatesComp->CrossStateData.IsComboActive) {
		this->StatesComp->CrossStateData.IsInterruptable = true;
		this->StatesComp->CrossStateData.IsComboActive = false;

		if (this->isCharged) {
			this->QueueNextAttack(EAttackMovementType::AM_CHARGED);
		}
		else if (this->isHeavy) {
			this->QueueNextAttack(EAttackMovementType::AM_HEAVY);
		}
		else {
			this->QueueNextAttack(EAttackMovementType::AM_DEFAULT);
		}
	}

	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable ) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}

	if (Action == EActionList::ActionBlock && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Block"), EActionList::ActionBlock, IE_Pressed);
	}
}


void UAttackState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->StartAttack();

}

void UAttackState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);


	//Note: Unbind ended/blendingout delegate if interrupted by state changes
	FAnimMontageInstance* AnimMontageInstance = this->StatesComp->CompContext->CharacterAnim->GetActiveInstanceForMontage(this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}

	this->StatesComp->CompContext->CombatModule.CurrentAttack = FAttackValues();
	this->StatesComp->CrossStateData.IsComboActive = true;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.25);
	this->StatesComp->CompContext->CharacterAnim->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->QueueTimer);

	if (this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		IEquipmentSystemAvailable::Execute_DisableWeaponDamageColliders(this->StatesComp->CompContext->CharacterActor);
	}
}

void UAttackState::StartAttack()
{
	this->StatesComp->CompContext->CharacterAnim->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->QueueTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HeavyTimer, this, &UAttackState::OnHeavyAttackActivated, HEAVYATTACKTRESHOLD, false, -1.0f);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->ChargeTimer, this, &UAttackState::OnChargeAttackActivated, CHARGEATTACKTRESHOLD, false, -1.0f);
	//sponge: should get from other later (maybe unarmed combat?)
	if (!this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_DEFAULT, this->StatesComp->CompContext->CombatModule.CurrentAttack, NewAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(NewAttack.AttackMontage);

}

void UAttackState::OnHeavyAttackActivated()
{
	this->isHeavy = true;

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_HEAVY, this->StatesComp->CompContext->CombatModule.CurrentAttack, NewAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(NewAttack.AttackMontage);
}

void UAttackState::OnChargeAttackActivated()
{
	this->isCharged = true;

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_CHARGED, this->StatesComp->CompContext->CombatModule.CurrentAttack, NewAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(NewAttack.AttackMontage);
	this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("Start", NewAttack.AttackMontage);

}

void UAttackState::QueueNextAttack(TEnumAsByte<EAttackMovementType> AttackType)
{
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->QueueTimer);

	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, AttackType, this->StatesComp->CompContext->CombatModule.CurrentAttack, this->StatesComp->CompContext->CombatModule.CurrentAttack);
	float StartTime = 0.0f;
	float EndTime = 0.0f;
	float CurrentPosition = this->StatesComp->CompContext->CharacterAnim->Montage_GetPosition(this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage->GetSectionStartAndEndTime(this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage->GetSectionIndex("Charge"), StartTime, EndTime);
	float WaitTime = CurrentPosition < StartTime ? StartTime - CurrentPosition : 0;
	if (WaitTime) {
		this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->QueueTimer, this, &UAttackState::DoAttack, WaitTime, false, -1.0f);
	}
	else {
		this->DoAttack();
	}
}


void UAttackState::DoAttack() {
	this->StatesComp->CompContext->CharacterAnim->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	//Change Moving Values Attribute SPONGE: maybe need to reset after exit attack state?
	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		float StaminaConsumption = this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()
			? IEquipmentSystemAvailable::Execute_GetCurrentAttackStaminaUsage(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CombatModule.CurrentAttack) : 0;
		ICharacterSystemAvailable::Execute_InitEffectDepleteStamina(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CharacterActor, StaminaConsumption);
	}


	float MontageLength = this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	if (MontageLength) {
		this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("Attack", this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
		FOnMontageEnded EndAttackDelegate;
		EndAttackDelegate.BindUObject(this, &UAttackState::OnAttackEnd);
		this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
		this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	}


	this->isHeavy = false;
	this->isCharged = false;
}

void UAttackState::OnAttackEnd(UAnimMontage* Montage, bool bInterrupted) {
	if (!bInterrupted) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	}
	else {
		this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);
	}
}