// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "InAirState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UInAirState : public UBaseState
{
	GENERATED_BODY()
	
public:
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
};
