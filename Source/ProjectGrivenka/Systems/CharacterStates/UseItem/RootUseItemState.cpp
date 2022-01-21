// Fill out your copyright notice in the Description page of Project Settings.


#include "RootUseItemState.h"
#include "OneOffUseItemState.h"
#include "HoldUseItemState.h"
#include "ToggleUseItemState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ProjectGrivenka/Items/BaseItem.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

//sponge: need to accomodate cases for actors without inventory comp
bool URootUseItemState::StateValidation_Implementation()
{
	return this->StatesComp->CompContext->CharacterActor->Implements<UCharacterInventoryAvailable>() 
		&& ICharacterInventoryAvailable::Execute_GetSelectedItemCount(this->StatesComp->CompContext->CharacterActor) > 0;
}

void URootUseItemState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	GLog->Log("rootuse action handler");
	if (this->CurrentUseItemState) {
		this->CurrentUseItemState->ActionHandler_Implementation(Action, EventType);
		GLog->Log("after cirrentuseitemstate action handler");
	}
	GLog->Log("After if rootuse action handler");

}

void URootUseItemState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{

	if (Action == EActionList::ActionMoveForward || Action == EActionList::ActionMoveRight)
	{
		this->StatesComp->CompContext->MovementComp->AddInputVector(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir);
	}
}

void URootUseItemState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);

	ICharacterInventoryAvailable::Execute_CommitItem(this->StatesComp->CompContext->CharacterActor);
	switch (ICharacterInventoryAvailable::Execute_GetCommitedItem(this->StatesComp->CompContext->CharacterActor)->ItemInfo.UsageType)
	{
		case EItemUsageType::IUT_ONEOFF:
			this->CurrentUseItemState = NewObject<UOneOffUseItemState>(this);
			break;
		case EItemUsageType::IUT_TOGGLE:
			this->CurrentUseItemState = NewObject<UToggleUseItemState>(this);
			break;
		case EItemUsageType::IUT_HOLD:
			this->CurrentUseItemState = NewObject<UHoldUseItemState>(this);
			break;
		default:
			break;
	}

	this->CurrentUseItemState->Init_Implementation(this->StatesComp);
	this->CurrentUseItemState->OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
}

void URootUseItemState::OnStateExit_Implementation()
{
	if(this->CurrentUseItemState) this->CurrentUseItemState->OnStateExit_Implementation();
}
