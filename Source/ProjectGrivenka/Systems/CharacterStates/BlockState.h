// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "BlockState.generated.h"

/**
 * 
 */

//Sponge: parry window is based on seconds, should probably be a frame?
static const float PARRYTIME = 0.15f;

UCLASS()
class PROJECTGRIVENKA_API UBlockState : public UBaseState
{
	GENERATED_BODY()

	TMap<TEnumAsByte<EHitDirectionType>, FName> ParryMap = {
		{ EHitDirectionType::TOPLEFT, "ParryRight" },
		{ EHitDirectionType::TOPRIGHT, "ParryLeft" },
		{ EHitDirectionType::LEFT, "ParryRight" },
		{ EHitDirectionType::RIGHT, "ParryLeft"},
		{ EHitDirectionType::BOTTOMRIGHT, "ParryLeft"},
		{ EHitDirectionType::BOTTOMLEFT, "ParryRight" },
	};

public:
	float TempMaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* HitInstigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo DamageInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LowImpactLaunchForce = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MediumImpactLaunchForce = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HighImpactLaunchForce = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosiveImpactLaunchForce = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Deceleration = 1000.0f;

	FTimerHandle ParryTimer;
	FTimerHandle SkidTimer;

public:
	UFUNCTION(BlueprintCallable)
	void OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo);
	void InvalidateParry();
	virtual bool StateValidation_Implementation() override;
	virtual void OnStateEnter_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue) override;
	virtual void OnStateExit_Implementation() override;

};
