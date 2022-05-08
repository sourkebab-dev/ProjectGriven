// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "BlockPushState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UBlockPushState : public UBaseState
{
	GENERATED_BODY()

	FTimerHandle PushTimer;

public:
	virtual bool StateValidation_Implementation() override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
	
	UFUNCTION()
	void OnBlockPushTriggered(enum EAnimEvt EventType);

};
