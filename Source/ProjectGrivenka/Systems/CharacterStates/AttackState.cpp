// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackState.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"


//SPONGE: SHOULD ADD OBJ MEMBER ACCESS VALIDATIONS
bool UAttackState::StateValidation_Implementation() {
	return this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void UAttackState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{	
	bool IsStaminaAllowed = this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor,EAttributeCode::ATT_Stamina) > 0 : true;

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
	//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);


	//Note: Unbind ended/blendingout delegate if interrupted by state changes
	FAnimMontageInstance* AnimMontageInstance = this->CharacterContext.CharacterAnim->GetActiveInstanceForMontage(this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}

	this->CharacterContext.Store->CombatModule.CurrentAttack = FAttackValues();
	this->StatesComp->CrossStateData.IsComboActive = true;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->CharacterContext.CharacterAnim->StopAllMontages(0.25);
	this->CharacterContext.CharacterAnim->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->QueueTimer);

	if (this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		IEquipmentSystemAvailable::Execute_DisableWeaponDamageColliders(this->CharacterContext.CharacterActor);
	}
}

void UAttackState::StartAttack()
{
	this->CharacterContext.CharacterAnim->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->QueueTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->HeavyTimer, this, &UAttackState::OnHeavyAttackActivated, HEAVYATTACKTRESHOLD, false, -1.0f);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->ChargeTimer, this, &UAttackState::OnChargeAttackActivated, CHARGEATTACKTRESHOLD, false, -1.0f);
	//sponge: should get from other later (maybe unarmed combat?)
	if (!this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_DEFAULT, this->CharacterContext.Store->CombatModule.CurrentAttack, NewAttack);
	this->CharacterContext.CharacterAnim->Montage_Play(NewAttack.AttackMontage);

}

void UAttackState::OnHeavyAttackActivated()
{
	this->isHeavy = true;

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_HEAVY, this->CharacterContext.Store->CombatModule.CurrentAttack, NewAttack);
	this->CharacterContext.CharacterAnim->Montage_Play(NewAttack.AttackMontage);
}

void UAttackState::OnChargeAttackActivated()
{
	this->isCharged = true;

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_CHARGED, this->CharacterContext.Store->CombatModule.CurrentAttack, NewAttack);
	this->CharacterContext.CharacterAnim->Montage_Play(NewAttack.AttackMontage);
	this->CharacterContext.CharacterAnim->Montage_JumpToSection("Start", NewAttack.AttackMontage);

}

void UAttackState::QueueNextAttack(TEnumAsByte<EAttackMovementType> AttackType)
{
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->QueueTimer);

	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, AttackType, this->CharacterContext.Store->CombatModule.CurrentAttack, this->CharacterContext.Store->CombatModule.CurrentAttack);
	float StartTime = 0.0f;
	float EndTime = 0.0f;
	float CurrentPosition = this->CharacterContext.CharacterAnim->Montage_GetPosition(this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage);
	this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage->GetSectionStartAndEndTime(this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage->GetSectionIndex("Charge"), StartTime, EndTime);
	float WaitTime = CurrentPosition < StartTime ? StartTime - CurrentPosition : 0;
	if (WaitTime) {
		this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->QueueTimer, this, &UAttackState::DoAttack, WaitTime, false, -1.0f);
	}
	else {
		this->DoAttack();
	}
}


void UAttackState::DoAttack() {
	this->CharacterContext.CharacterAnim->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	//Change Moving Values Attribute SPONGE: maybe need to reset after exit attack state?
	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		float StaminaConsumption = this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>()
			? IEquipmentSystemAvailable::Execute_GetCurrentAttackStaminaUsage(this->CharacterContext.CharacterActor, this->CharacterContext.Store->CombatModule.CurrentAttack) : 0;
		ICharacterSystemAvailable::Execute_InitEffectDepleteStamina(this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor, StaminaConsumption);
	}


	float MontageLength = this->CharacterContext.CharacterAnim->Montage_Play(this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage);
	if (MontageLength) {
		this->CharacterContext.CharacterAnim->Montage_JumpToSection("Attack", this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage);
		FOnMontageEnded EndAttackDelegate;
		EndAttackDelegate.BindUObject(this, &UAttackState::OnAttackEnd);
		this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage);
		this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage);
	}


	this->isHeavy = false;
	this->isCharged = false;
}

void UAttackState::OnAttackEnd(UAnimMontage* Montage, bool bInterrupted) {
	if (!bInterrupted) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	}
	else {
		//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
		//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
	}
}