// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "JumpAttackState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UJumpAttackState : public UBaseState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	class UCharacterMovementComponent* CharMove;


public:
	virtual bool StateValidation_Implementation() override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
	UFUNCTION()
	void OnAttackEnd(UAnimMontage* Montage, bool bInterrupted);
};
