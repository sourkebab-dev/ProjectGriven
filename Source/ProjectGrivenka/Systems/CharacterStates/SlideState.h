// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "SlideState.generated.h"

/**
 * 
 */
//sponge: probably need to make this a dynamic (changeable in engine)
static const float TERMINALVELOCITY = 1100.0f;
static const float MINSLIDEVELOCITY = 300.0f;

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API USlideState : public UBaseState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	class UCharacterMovementComponent* CharMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* SlideMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> HitList;

public:
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void Tick_Implementation(float DeltaTime);
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
};
