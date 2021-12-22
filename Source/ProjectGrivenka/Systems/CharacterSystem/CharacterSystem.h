// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "CharacterSystemDefinitions.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
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
	void InitializeAttributes(FPersisted_CharacterAttributes PersistedAttributes);
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
	void InitFXByPrefab(AActor* EffectReceiver, AActor* EffectInstigator, class UEffectPrefab* InEffectPrefab);
	UFUNCTION(BlueprintCallable)
	void InitFXReceiveHit(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FVector InDamageDirection, TEnumAsByte<enum EDamageImpactType> InImpactType);
	UFUNCTION(BlueprintCallable)
	void InitFXDepleteStamina(float InValue, AActor* EffectReceiver, AActor* EffectInstigator);
	UFUNCTION(BlueprintCallable)
	void InitFXStartAmpRegen(float InValue, AActor* EffectReceiver, AActor* EffectInstigator);
	UFUNCTION(BlueprintCallable)
	void InitFXSetWeaponDamage(float InValue, AActor* EffectReceiver, AActor* EffectInstigator);
	UFUNCTION(BlueprintCallable)
	void InitFXSetWeaponMovingValues(float InValue, AActor* EffectReceiver, AActor* EffectInstigator);
	#pragma endregion
		
};
