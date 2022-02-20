// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "KnockedDownState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UKnockedDownState : public UBaseState
{
	GENERATED_BODY()

	bool IsRecovering = false;
	FTimerHandle RecoverTimer;

public:
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;

};
