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
	void OnFinishLunge();

public:
	virtual void Init_Implementation(FCharacterContext InContext, class UCharacterStatesSystem* InStatesComp) override;
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void OnStateExit_Implementation() override;

};