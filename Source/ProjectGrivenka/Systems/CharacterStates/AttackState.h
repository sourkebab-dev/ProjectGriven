// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "AttackState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UAttackState : public UBaseState
{
	GENERATED_BODY()

	float HeavyAttackStartSectionLength = 0.0;
	float ChargedAttackStartSectionLength = 0.0;
	float PooledChargingTime = 0.0;
	bool isHolding = false;
	bool isCharged = false;
	FTimerHandle ChargeTimer;
	FAttackValues CurrentAttack;

public:
	virtual void Init_Implementation(FCharacterContext InContext, class UCharacterStatesSystem* InStatesComp) override;
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void OnStateExit_Implementation() override;

	void DoAttack(TEnumAsByte<EAttackMovementType> AttackType);

	UFUNCTION()
	void OnAttackEnd(class UAnimMontage* Montage, bool bInterrupted);

	void StartAttackCharge();
	void OnAttackCharging();
};
