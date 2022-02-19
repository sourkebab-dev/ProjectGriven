// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "StaggeredState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UStaggeredState : public UBaseState
{
	GENERATED_BODY()

	FTimerHandle HitStopTimer;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* StaggerMontage;

public:
	UFUNCTION(BlueprintCallable)
	void OnStaggerEnd(UAnimMontage* Montage, bool bInterrupted);
	virtual void OnStateEnter_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue) override;
	virtual void OnStateExit_Implementation() override;


};
