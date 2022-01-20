// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameplayTagContainer.h"
#include "EventBus.generated.h"

UENUM(BlueprintType)
enum EAnimEvt {
	ACTIVATE_INTERRUPTION,
	DISABLE_INTERRUPTION,
	ACTIVATE_COMBO,
	DISABLE_COMBO,
	FULL_ROTATION,
	SLOW_ROTATION,
	OFF_ROTATION,
	START_DODGE,
	END_DODGE,
};

DECLARE_DELEGATE(FHitStopFinishDelegate);
DECLARE_DELEGATE_TwoParams(FHitStopStartDelegate, EDamageImpactType, FHitStopFinishDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLatentInteractionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAnimDelegate, EAnimEvt, EventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamagedDelegate, AActor*, DamageInstigator, FDamageInfo, DamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemUsageDelegate, AActor*, UsageInstigator, class UEffectPrefab*, EffectPrefab);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateAxisDelegate, EActionList, InActionType, float, InValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateActionDelegate, EActionList, InActionType, EInputEvent, InEventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossessionDelegate, AController*, NewController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnpossessionDelegate,  AController*, OldController);

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UEventBus : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDamagedDelegate StaggerDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDamagedDelegate DamagedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FItemUsageDelegate ItemUsageDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FLatentInteractionDelegate InteractionDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStateActionDelegate StateActionDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStateAxisDelegate StateAxisDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FPossessionDelegate PossessionDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FUnpossessionDelegate UnpossesionDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAnimDelegate AnimDelegate;

	FHitStopStartDelegate HitStopDelegate;
};