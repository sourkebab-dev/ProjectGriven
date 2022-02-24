// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterSystemDefinitions.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "CharacterSystemAvailable.generated.h"

USTRUCT(BlueprintType)
struct FPersistedAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
	float MaxAmp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
	float Amp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
	float AmpRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
	float MaxFortitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
	float Fortitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
	float FortitudeRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float CriticalChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float CriticalPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
	float ElemFireDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
	float ElemIceDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
	float ElemElectricDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
	float ElemPoisonDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
	float ElemEnergyDefense;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCharacterSystemAvailable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API ICharacterSystemAvailable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UCharacterSystem* GetCharacterSystemComp();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LoadAttributes(FPersistedAttributes InAttributes);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitEffectByPrefab(AActor* EffectInstigator, class UEffectPrefab* InEffectPrefab, float InOverrideValue = 0.0f, bool IsValueOverridden = false );

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitEffectByPrefabName(AActor* EffectInstigator, FName InPrefabName, float InOverrideValue = 0.0f, bool IsValueOverridden = false);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitEffectByRegenName(AActor* EffectInstigator, FName InPrefabName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitEffectDepleteStamina(AActor* EffectInstigator, float InValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitEffectReceiveHit(AActor* EffectInstigator, FDamageInfo InDamageInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitEffectFortitudeDamage(AActor* EffectInstigator, FDamageInfo InDamageInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveEffectByTag(FGameplayTag EffectTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveEffectById(FName EffectId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetAttributeCurrentValue(EAttributeCode InAttributeCode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetAttributeMaxValue(EAttributeCode InAttributeCode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SubscribeAttributeChanges(EAttributeCode InAttributeCode, const FSimpleDynamicDelegate& InDelegate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnSubscribeAttributeChanges(EAttributeCode InAttributeCode, const FSimpleDynamicDelegate& InDelegate);
};
