// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "JumpState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UJumpState : public UBaseState
{
	GENERATED_BODY()

	float ToApexDuration;

public:
	UPROPERTY(BlueprintReadOnly)
	class UCharacterMovementComponent* CharMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* JumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpZVelocity = 1500.0f;

public:
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void Tick_Implementation(float DeltaTime);
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;

	UFUNCTION()
	void OnJumpLaunchedTriggered(enum EAnimEvt EventType);

};