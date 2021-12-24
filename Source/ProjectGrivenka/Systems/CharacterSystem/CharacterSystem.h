// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "CharacterSystemDefinitions.h"
#include "CharacterSystemAvailable.h"
#include "CharacterSystem.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UCharacterSystem : public UBaseContextableComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCharacterPrefab* PrefabData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCharacterSystemAttributes* Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UBaseEffect*> PassiveEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UBaseEffect*> ActiveEffects;
	
protected:
	virtual void BeginPlay() override;

public:	
	void Init();
	UFUNCTION(BlueprintCallable)
	void InitializeAttributes(FPersistedAttributes PersistedAttributes);
	UFUNCTION(BlueprintCallable)
	void AddEffect(class UBaseEffect* InEffect);
	UFUNCTION(BlueprintCallable)
	void RemoveEffect(class UBaseEffect* InEffect);
	UFUNCTION(BlueprintCallable)
	void RemoveEffectsByTag(FGameplayTag EffectTag);
	UFUNCTION(BlueprintCallable)
	void RemoveEffectsById(FName EffectId);
	UFUNCTION(BlueprintCallable)
	void DeactivateAllEffects();

	//Sponge: maybe should use events for all of these?
	#pragma region Primitive GetterSetters (should be called internally in the system)
	UFUNCTION(BlueprintCallable)
	float GetAttributeBaseValue(TEnumAsByte<EAttributeCode> InAttributeCode);
	UFUNCTION(BlueprintCallable)
	float GetAttributeMaxValue(TEnumAsByte<EAttributeCode> InAttributeCode);
	UFUNCTION(BlueprintCallable)
	float GetAttributeCurrentValue(TEnumAsByte<EAttributeCode> InAttributeCode);
	UFUNCTION(BlueprintCallable)
	void SetAttributeValue(TEnumAsByte<EAttributeCode> InAttributeCode, float Value);
	#pragma endregion


	#pragma region Effects Initiators
	UFUNCTION(BlueprintCallable)
	void InitEffectByPrefab(AActor* EffectInstigator, class UEffectPrefab* InEffectPrefab, float InOverrideValue = 0.0f, bool IsValueOverridden = false);
	UFUNCTION(BlueprintCallable)
	void InitEffectByPrefabName(AActor* EffectInstigator, FName InPrefabName, float InOverrideValue = 0.0f, bool IsValueOverridden = false);
	UFUNCTION(BlueprintCallable)
	void InitEffectByRegenName(AActor* EffectInstigator, FName InPrefabName);
	UFUNCTION(BlueprintCallable)
	void InitEffectDepleteStamina(AActor* EffectInstigator, float InValue);
	UFUNCTION(BlueprintCallable)
	void InitEffectReceiveHit(AActor* EffectInstigator, FVector InDamageDirection, TEnumAsByte<enum EDamageImpactType> InImpactType);
	#pragma endregion
		
};