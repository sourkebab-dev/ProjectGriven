// Fill out your copyright notice in the Description page of Project Settings.


#include "MotionState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"

void UMotionState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	bool IsStaminaAllowed = this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->StatesComp->CompContext->CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;

	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && IsStaminaAllowed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}

	if (Action == EActionList::ActionAttack && EventType == IE_Pressed && IsStaminaAllowed) {
		if (this->StatesComp->CrossStateData.IsLungeAvailable) {
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Attack.Lunge"), EActionList::ActionAttack, IE_Pressed);
		}
		else {
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Attack"), EActionList::ActionAttack, IE_Pressed);
		}
	}

	if (Action == EActionList::ActionToggleAmpField && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.ToggleAmp"), EActionList::ActionToggleAmpField, IE_Pressed);
	}

	if (Action == EActionList::ActionUseItem && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.ItemUse"), EActionList::ActionUseItem, IE_Pressed);

	}

	if (Action == EActionList::ActionInteract && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Interact"), EActionList::ActionInteract, IE_Pressed);
	}

	if (Action == EActionList::ActionBlock && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Block"), EActionList::ActionBlock, IE_Pressed);
	}


	if (Action == EActionList::ActionJump && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Jump"), EActionList::ActionJump, IE_Pressed);
	}
}

void UMotionState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{

	if (Action == EActionList::ActionMoveForward || Action == EActionList::ActionMoveRight)
	{
		this->MoveCharacter();
	}

}

void UMotionState::MoveCharacter() {
	if (!this->StatesComp->CompContext->MovementComp) return;
	this->StatesComp->CompContext->MovementComp->AddInputVector(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir);
}

void UMotionState::OnStateEnter_Implementation() {
	Super::OnStateEnter_Implementation();
	if (this->StatesComp->CrossStateData.IsLungePooling) {
		this->StatesComp->CrossStateData.IsLungePooling = false;
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Attack.Lunge"), EActionList::ActionNone, IE_Released);
	}
}
