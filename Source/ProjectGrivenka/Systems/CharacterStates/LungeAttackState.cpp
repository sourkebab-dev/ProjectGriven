// Fill out your copyright notice in the Description page of Project Settings.


#include "LungeAttackState.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"

void ULungeAttackState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
}

bool ULungeAttackState::StateValidation_Implementation() {
	return this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
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

void ULungeAttackState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CrossStateData.IsComboActive = false;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_LUNGE, FAttackValues(), this->CharacterContext.Store->CombatModule.CurrentAttack);
	UAnimMontage* LungeMontage = this->CharacterContext.Store->CombatModule.CurrentAttack.AttackMontage;
	float MontageLength = this->CharacterContext.CharacterAnim->Montage_Play(LungeMontage);
	if (MontageLength) {
		this->LungeEndDelegate.BindUObject(this, &ULungeAttackState::OnFinishLunge);
		this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(this->LungeEndDelegate, LungeMontage);
		this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(this->LungeEndDelegate, LungeMontage);
	}

	this->StatesComp->CrossStateData.IsLungeAvailable = false;

	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		float StaminaConsumption = this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>() ? IEquipmentSystemAvailable::Execute_GetCurrentAttackStaminaUsage(this->CharacterContext.CharacterActor, this->CharacterContext.Store->CombatModule.CurrentAttack) : 0;
		ICharacterSystemAvailable::Execute_InitEffectDepleteStamina(this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor, StaminaConsumption);
	}
}

void ULungeAttackState::OnStateExit_Implementation()
{
	//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);

	//Note: Unbind ended/blendingout delegate if interrupted by state changes
	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_LUNGE, FAttackValues(), NewAttack);
	UAnimMontage* LungeMontage = NewAttack.AttackMontage;
	FAnimMontageInstance* AnimMontageInstance = this->CharacterContext.CharacterAnim->GetActiveInstanceForMontage(LungeMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->CharacterContext.Store->CombatModule.CurrentAttack = FAttackValues();
	this->StatesComp->CrossStateData.IsComboActive = true;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->CharacterContext.CharacterAnim->StopAllMontages(0.25);
	if (this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		IEquipmentSystemAvailable::Execute_DisableWeaponDamageColliders(this->CharacterContext.CharacterActor);
	}
}
