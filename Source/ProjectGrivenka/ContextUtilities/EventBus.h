// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameplayTagContainer.h"
#include "EventBus.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLatentInteractionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDamagedDelegate, AActor*, DamageInstigator,  FVector, DamageDirection, EDamageImpactType, ImpactType);
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
};