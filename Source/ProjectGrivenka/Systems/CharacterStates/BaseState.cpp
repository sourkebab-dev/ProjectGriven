// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseState.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"

bool UBaseState::StateValidation_Implementation()
{
	return true;
}

void UBaseState::Init_Implementation(UCharacterStatesSystem* InStatesComp)
{
	this->StatesComp = InStatesComp;
}

void UBaseState::OnStateEnter_Implementation()
{
}

void UBaseState::Tick_Implementation(float DeltaTime)
{
}

void UBaseState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
}

void UBaseState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
}


void UBaseState::OnStateExit_Implementation()
{
}
