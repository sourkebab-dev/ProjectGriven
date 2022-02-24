// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraitSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "TraitSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UTraitSystem : public UBaseContextableComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> TraitIds;

public:
	UFUNCTION(BlueprintCallable)
		void LoadTraits(FPersistedTraitData InTraitData);

	UFUNCTION(BlueprintCallable)
		void AddTrait(FName InTraitId);

	UFUNCTION(BlueprintCallable)
		void ActivateTrait(FName InTraitId);

	UFUNCTION(BlueprintCallable)
		void DisableTrait(FName InTraitId);

	UFUNCTION(BlueprintCallable)
		void RemoveTrait(FName InTraitId);
};