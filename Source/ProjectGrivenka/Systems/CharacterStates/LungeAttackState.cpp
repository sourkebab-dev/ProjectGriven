// Fill out your copyright notice in the Description page of Project Settings.


#include "LungeAttackState.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"

bool ULungeAttackState::StateValidation_Implementation() {
	return this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->StatesComp->CompContext->CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void ULungeAttackState::OnFinishLunge(UAnimMontage* Montage, bool bInterrupted)
{
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
}

void ULungeAttackState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionAttack && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsComboActive) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Attack"), EActionList::ActionAttack, IE_Pressed);
	}
}

void ULungeAttackState::OnStateEnter_Implementation()
{
	Super::OnStateEnter_Implementation();
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CrossStateData.IsComboActive = false;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_LUNGE, FAttackValues(), this->StatesComp->CompContext->CombatModule.CurrentAttack);
	UAnimMontage* LungeMontage = this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage;
	float MontageLength = this->StatesComp->CompContext->CharacterAnim->Montage_Play(LungeMontage);
	if (MontageLength) {
		this->LungeEndDelegate.BindUObject(this, &ULungeAttackState::OnFinishLunge);
		this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(this->LungeEndDelegate, LungeMontage);
		this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(this->LungeEndDelegate, LungeMontage);
	}
     
	this->StatesComp->CrossStateData.IsLungeAvailable = false;
	
	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		float StaminaConsumption = this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>() ? IEquipmentSystemAvailable::Execute_GetCurrentAttackStaminaUsage(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CombatModule.CurrentAttack) : 0;
		ICharacterSystemAvailable::Execute_InitEffectDepleteStamina(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CharacterActor, StaminaConsumption);
	}
}

void ULungeAttackState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);

	//Note: Unbind ended/blendingout delegate if interrupted by state changes
	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_LUNGE, FAttackValues(), NewAttack);
	UAnimMontage* LungeMontage = NewAttack.AttackMontage;
	FAnimMontageInstance* AnimMontageInstance = this->StatesComp->CompContext->CharacterAnim->GetActiveInstanceForMontage(LungeMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->StatesComp->CompContext->CombatModule.CurrentAttack = FAttackValues();
	this->StatesComp->CrossStateData.IsComboActive = true;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.25);
	if (this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		IEquipmentSystemAvailable::Execute_DisableWeaponDamageColliders(this->StatesComp->CompContext->CharacterActor);
	}
}
