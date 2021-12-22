// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameplayTagContainer.h"
#include "EventBus.generated.h"

//Note: List of simple delegates that are activated without an instigator
UENUM(BlueprintType)
enum EContextDelegates
{
	CDL_WEAPON_COLLIDE_DISABLE UMETA(DisplayName ="Weapon Collide Disable"),
	CDL_INTERACT UMETA(DisplayName= "Interaction Start/End"),
	CDL_COMMIT_ITEM UMETA(DisplayName = "Commit Use Item"),
	CDL_USEITEM UMETA(DisplayName= "Use Item"),
};

//Note: List of simple delegates that are activated with an instigator
UENUM(BlueprintType)
enum EInstigatedDelegate
{
	CDL_INTERACTED UMETA(DisplayName = "Being Interacted With"),
};

//Note: List delegates that is used to initiate an RPG Effect
UENUM(BlueprintType)
enum EEffectDelegates
{
	EDL_DEPLETE_STAMINA UMETA(DisplayName = "Deplete Stamina Delegate"),
	EDL_START_AMP_REGEN UMETA(DisplayName = "Start Amp Regen"),
	EDL_WEAPON_DAMAGE UMETA(DisplayName = "Weapon Damage Delegate"),
	EDL_MOVING_VALUES UMETA(DisplayName = "Weapon Moving Values Delegate"),
};

DECLARE_MULTICAST_DELEGATE_OneParam(FInstigatedDelegate, AActor*);
DECLARE_MULTICAST_DELEGATE(FContextSimpleDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FRPGEffectRemoveDelegate, FGameplayTag);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FRPGEffectInitDelegate, float, AActor*, AActor*);
DECLARE_MULTICAST_DELEGATE_FourParams(FDamagedDelegate, AActor*, AActor*, FVector, TEnumAsByte<enum EDamageImpactType>);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FItemUsageDelegate, AActor*, AActor*, class UEffectPrefab*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateAxisDelegate, enum EActionList, InActionType, float, InValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateActionDelegate, enum EActionList, InActionType, EInputEvent, InEventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossessionDelegate, AController*, NewController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnpossessionDelegate,  AController*, OldController);

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UEventBus : public UObject
{
	GENERATED_BODY()
	
public:
	#pragma region Non-System Delegates
	FDamagedDelegate DamagedDelegate;
	FItemUsageDelegate ItemUsageDelegate;
	TMap<TEnumAsByte<EContextDelegates>, FContextSimpleDelegate> Observers;
	TMap<TEnumAsByte<EInstigatedDelegate>, FInstigatedDelegate> InstigatedObservers;
	#pragma endregion

	#pragma region Char System Delegates
	FRPGEffectRemoveDelegate EffectRemoveDelegate;
	TMap<TEnumAsByte<EEffectDelegates>, FRPGEffectInitDelegate> EffectApplyObservers;
	#pragma endregion

	#pragma region Equipment System Delegates

	#pragma endregion

	#pragma region State System Delegates
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStateActionDelegate StateActionDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FStateAxisDelegate StateAxisDelegate;
	#pragma endregion

	#pragma region Control System Delegates
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FPossessionDelegate PossessionDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FUnpossessionDelegate UnpossesionDelegate;
	#pragma endregion


public:
	void AddInstigatedEvtSubscriber(FInstigatedDelegate& OutDelegate,TEnumAsByte<EInstigatedDelegate> InEventCode);

	void AddSubscriber(FContextSimpleDelegate& OutDelegate, TEnumAsByte<EContextDelegates> InEventCode);

	void AddEffectInitSubscriber(FRPGEffectInitDelegate& OutDelegate, TEnumAsByte<EEffectDelegates> InEffectInitCode);

};