// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "LungeAttackState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API ULungeAttackState : public UBaseState
{
	GENERATED_BODY()

	UFUNCTION()
	void OnFinishLunge(UAnimMontage* Montage, bool bInterrupted);

	FOnMontageEnded LungeEndDelegate;

public:
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
};
