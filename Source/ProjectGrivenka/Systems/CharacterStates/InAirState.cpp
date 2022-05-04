// Fill out your copyright notice in the Description page of Project Settings.


#include "InAirState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

void UInAirState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionAttack && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Attack.Air"), EActionList::ActionNone, IE_Released);
	}
}

void UInAirState::OnStateEnter_Implementation()
{
}

void UInAirState::OnStateExit_Implementation()
{
}

