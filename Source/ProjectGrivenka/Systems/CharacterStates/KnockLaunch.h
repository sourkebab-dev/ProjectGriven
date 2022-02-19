// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "KnockLaunch.generated.h"


static const float PEAKSTEPPER = 0.05;


//Sponge: might need to move this to data asset
USTRUCT(BlueprintType)
struct FLaunchData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* LaunchMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCurveVector* LaunchCurve;
};

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UKnockLaunch : public UBaseState
{
	GENERATED_BODY()
	
	FTimerHandle HitPauseTimer;
	FVector LaunchStartLocation;
	float PooledLaunchTime = 0.0f;
	float PeakTime;
	bool IsProcessLaunch = false;
	FSimpleDynamicDelegate LaunchApexDelegate;
	FSimpleDynamicDelegate HitGroundDelegate;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLaunchData> LaunchBack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLaunchData> LaunchFront;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLaunchData> LaunchUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLaunchData SelectedLaunch;

public:
	UFUNCTION()
	void OnLaunchEnd(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnLaunchApex();
	UFUNCTION()
	void OnHitGround();
	float FindPeakTime(int MultiplierStart, int MultiplierEnd);
	virtual void Tick_Implementation(float DeltaTime);
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation() override;
	virtual void OnStateExit_Implementation() override;
};
