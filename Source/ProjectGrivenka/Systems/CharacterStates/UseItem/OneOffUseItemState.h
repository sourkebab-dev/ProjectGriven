// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/CharacterStates/BaseState.h"
#include "OneOffUseItemState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UOneOffUseItemState : public UBaseState
{
	GENERATED_BODY()

	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
	UFUNCTION()
	virtual void OnUseItemAnimFinished();
};
