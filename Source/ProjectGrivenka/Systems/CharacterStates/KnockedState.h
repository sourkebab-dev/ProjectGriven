// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "KnockedState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UKnockedState : public UBaseState
{
	GENERATED_BODY()

	float StunTime = 0.0;
	float PooledStunTime = 0.0;
	FTimerHandle StunTimer;

	UFUNCTION()
	void OnStunned();

	UFUNCTION()
	void OnFinishStunned();

public:
	//virtual void Init_Implementation(class ABaseCharacter* Instance) override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void OnStateExit_Implementation() override;
};
