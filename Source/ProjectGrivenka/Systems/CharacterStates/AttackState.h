// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "AttackState.generated.h"

/**
 * 
 */
static const float HEAVYATTACKTRESHOLD = 0.25;
static const float CHARGEATTACKTRESHOLD = 0.75;

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UAttackState : public UBaseState
{
	GENERATED_BODY()

	bool isHeavy = false;
	bool isCharged = false;
	FTimerHandle HeavyTimer;
	FTimerHandle ChargeTimer;

public:
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;


	UFUNCTION()
	void OnAttackEnd(class UAnimMontage* Montage, bool bInterrupted);

	void StartAttack();
	void OnHeavyAttackActivated();
	void OnChargeAttackActivated();
	void DoAttack(TEnumAsByte<EAttackMovementType> AttackType);
};
