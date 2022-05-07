// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "KnockedState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FKnockBackDataContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKnockBackData> Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKnockBackData> FarPush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKnockBackData> Launch;

};

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UKnockedState : public UBaseState
{
	GENERATED_BODY()


	FTimerHandle HitPauseTimer;

	TMap<TEnumAsByte<EHitDirectionType>, EHitDirectionType> InversionMap = {
		{ EHitDirectionType::TOPLEFT, EHitDirectionType::TOPRIGHT },
		{ EHitDirectionType::TOPRIGHT, EHitDirectionType::TOPLEFT },
		{ EHitDirectionType::LEFT, EHitDirectionType::RIGHT },
		{ EHitDirectionType::RIGHT, EHitDirectionType::LEFT },
		{ EHitDirectionType::BOTTOMRIGHT, EHitDirectionType::BOTTOMLEFT},
		{ EHitDirectionType::BOTTOMLEFT, EHitDirectionType::BOTTOMRIGHT },
		{ EHitDirectionType::FRONT, EHitDirectionType::BACK },
	};

	TMap<TEnumAsByte<EDamageImpactType>, int> ImpactModifier = {
		{ EDamageImpactType::DI_LOW, 1 },
		{ EDamageImpactType::DI_MEDIUM, 0 },
		{ EDamageImpactType::DI_HIGH, -1 },
		{ EDamageImpactType::DI_EXPLOSIVE, -2 },
	};

	float PooledTime = 0.0f;

public:
	bool IsFallingPartitioned = false;
	bool IsLandingPartitioned = false;
	bool IsCurveEnd = false;
	bool IsProcessKnockback = false;
	float ApexTime;
	float LaunchPlayRate = 1.0;
	float CachedAnimRate = 1.0;
	FVector ForceDirection;

	UPROPERTY(BlueprintReadWrite)
	class UCharacterMovementComponent* CharMove;
	UPROPERTY(BlueprintReadWrite)
	FKnockBackData CurrentKnockbackData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EHitDirectionType>, FKnockBackDataContainer> KnockbackData;

public:
	virtual void Tick_Implementation(float DeltaTime);
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
	

	UFUNCTION(BlueprintCallable)
	void StartHitReact();
	UFUNCTION(BlueprintCallable)
	void OnHitReactEnd(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnProcessAnimRate(enum EAnimEvt EventType);

};
