// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "Components/TimelineComponent.h"
#include "DodgeState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UDodgeState : public UBaseState
{
	GENERATED_BODY()

	float TempWalkSpeed;
	float TempAcceleration;
	float TempGroundFriction;
	float TempBrakingFriction;
	FVector TempCurrentLocation;
	FVector DodgeTargetLocation;
	FTimerHandle LungeTimer;
	FTimerHandle DodgeTimer;
	FTimeline DodgeTimeline;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveFloat* DodgeCurve;

public:
	virtual bool StateValidation_Implementation() override;
	virtual void Init_Implementation(UCharacterStatesSystem* InStatesComp) override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void OnStateExit_Implementation() override;
	
	UFUNCTION()
	void OnDodging(float InterpValue);

	UFUNCTION()
	void OnFinishDodging();

	UFUNCTION()
	void DeactivateLunge();

	UFUNCTION()
	void SetDamageCollider(bool IsActive);
};
