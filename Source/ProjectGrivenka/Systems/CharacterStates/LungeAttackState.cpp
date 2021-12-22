// Fill out your copyright notice in the Description page of Project Settings.


#include "LungeAttackState.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void ULungeAttackState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
}

bool ULungeAttackState::StateValidation_Implementation() {
	return this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void ULungeAttackState::OnFinishLunge()
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
	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_LUNGE, FAttackValues(), NewAttack);
	UAnimMontage* LungeMontage = NewAttack.AttackMontage;
	float MontageLength = this->CharacterContext.CharacterAnim->Montage_Play(LungeMontage);
	if (MontageLength) {
		FOnMontageEnded EndAttackDelegate;
		EndAttackDelegate.BindUFunction(this, "OnFinishLunge");
		this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, LungeMontage);
		this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, LungeMontage);
	}

	this->StatesComp->CrossStateData.IsLungeAvailable = false;
	float StaminaConsumption = IEquipmentSystemAvailable::Execute_GetCurrentAttackStaminaUsage(this->CharacterContext.CharacterActor, NewAttack);
	FRPGEffectInitDelegate* EffectInitDelegate = this->CharacterContext.EventBus->EffectApplyObservers.Find(EEffectDelegates::EDL_MOVING_VALUES);
	if (EffectInitDelegate) EffectInitDelegate->Broadcast(NewAttack.MovingValues, this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor);
	EffectInitDelegate = this->CharacterContext.EventBus->EffectApplyObservers.Find(EEffectDelegates::EDL_DEPLETE_STAMINA);
	if (EffectInitDelegate) EffectInitDelegate->Broadcast(StaminaConsumption, this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor);	
}

void ULungeAttackState::OnStateExit_Implementation()
{
	//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
	this->StatesComp->CrossStateData.IsComboActive = true;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->CharacterContext.CharacterAnim->StopAllMontages(0.25);
	FContextSimpleDelegate* ContextDelegates = this->CharacterContext.EventBus->Observers.Find(EContextDelegates::CDL_WEAPON_COLLIDE_DISABLE);
	if (ContextDelegates) ContextDelegates->Broadcast();
}
