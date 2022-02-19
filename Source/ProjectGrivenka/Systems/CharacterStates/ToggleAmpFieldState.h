// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "ToggleAmpFieldState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UToggleAmpFieldState : public UBaseState
{
	GENERATED_BODY()

	UFUNCTION()
	void RaiseAmpField();
	UFUNCTION()
	void LowerAmpField();
	void EndAmpAction();
	
public:
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
};
