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

	if (Action == EActionList::ActionAttack && EventType == IE_Pressed && IsStaminaAllowed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->StartAttack();
	}

	if (Action == EActionList::ActionAttack && EventType == IE_Released && IsStaminaAllowed) {
		this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
		this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);

		if (this->isCharged || this->isHeavy) {
			this->StatesComp->CrossStateData.IsInterruptable = false;
		}

		if (this->isCharged) {
			this->DoAttack(EAttackMovementType::AM_CHARGED);
		}
		else if (this->isHeavy) {
			this->DoAttack(EAttackMovementType::AM_HEAVY);
		}
		else if (this->StatesComp->CrossStateData.IsInterruptable) {
			this->StatesComp->CrossStateData.IsInterruptable = false;
			this->DoAttack(EAttackMovementType::AM_DEFAULT);
		}
	}

	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable ) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}

	if (Action == EActionList::ActionBlock && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Block"), EActionList::ActionBlock, IE_Pressed);
	}
}


void UAttackState::OnStateEnter_Implementation()
{
	Super::OnStateEnter_Implementation();
	this->StartAttack();
}

void UAttackState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);
	this->StatesComp->CompContext->CombatModule.CurrentAttack = FAttackValues();
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.25);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);

	if (this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		IEquipmentSystemAvailable::Execute_DisableWeaponDamageColliders(this->StatesComp->CompContext->CharacterActor);
	}
}

void UAttackState::StartAttack()
{
	this->isHeavy = false;
	this->isCharged = false;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HeavyTimer, this, &UAttackState::OnHeavyAttackActivated, HEAVYATTACKTRESHOLD, false, -1.0f);
	//sponge: should get from other later (maybe unarmed combat?)
	if (!this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_DEFAULT, this->StatesComp->CompContext->CombatModule.CurrentAttack, NewAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(NewAttack.AttackMontage);
	this->StatesComp->CompContext->CharacterAnim->Montage_Pause(this->StatesComp->CompContext->CharacterAnim->GetCurrentActiveMontage());
}

void UAttackState::OnHeavyAttackActivated()
{

	this->isHeavy = true;
	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_HEAVY, this->StatesComp->CompContext->CombatModule.CurrentAttack, NewAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(NewAttack.AttackMontage);
	float WindupTime = NewAttack.AttackMontage->GetSectionLength(NewAttack.AttackMontage->GetSectionIndex("Windup"));
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->ChargeTimer, this, &UAttackState::OnChargeAttackActivated, WindupTime, false, -1.0f);
}

void UAttackState::OnChargeAttackActivated()
{
	this->isCharged = true;
	this->StatesComp->CompContext->CharacterAnim->Montage_Pause(this->StatesComp->CompContext->CharacterAnim->GetCurrentActiveMontage());
}

void UAttackState::DoAttack(TEnumAsByte<EAttackMovementType> AttackType) {
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HeavyTimer);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);

	//Note: Sets the current selected attack
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, AttackType, this->StatesComp->CompContext->CombatModule.CurrentAttack, this->StatesComp->CompContext->CombatModule.CurrentAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Resume(this->StatesComp->CompContext->CharacterAnim->GetCurrentActiveMontage());
	if (AttackType == EAttackMovementType::AM_HEAVY) {
		this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage, 0.8f);
		this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("Attack", this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	}
	

	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UAttackState::OnAttackEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	this->isHeavy = false;
	this->isCharged = false;
}

void UAttackState::OnAttackEnd(UAnimMontage* Montage, bool bInterrupted) {
	if (!bInterrupted && !this->IsInvalidated) {
		this->IsInvalidated = true;
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	}
	else if (bInterrupted) {
		if (this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
			IEquipmentSystemAvailable::Execute_DisableWeaponDamageColliders(this->StatesComp->CompContext->CharacterActor);
		}
		this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);
	}
}