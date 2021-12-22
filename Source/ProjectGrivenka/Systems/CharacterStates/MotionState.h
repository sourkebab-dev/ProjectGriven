// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "MotionState.generated.h"

class ABaseCharacter;

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UMotionState : public UBaseState
{
	GENERATED_BODY()

public:
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue) override;
	virtual void MoveCharacter();

};
