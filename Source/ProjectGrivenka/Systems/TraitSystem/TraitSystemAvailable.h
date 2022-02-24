// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TraitSystemAvailable.generated.h"

USTRUCT(BlueprintType)
struct FPersistedTraitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> TraitIds;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTraitSystemAvailable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API ITraitSystemAvailable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LoadTraits(FPersistedTraitData InTraitData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddTrait(FName InTraitId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RemoveTrait(FName InTraitId);

};
