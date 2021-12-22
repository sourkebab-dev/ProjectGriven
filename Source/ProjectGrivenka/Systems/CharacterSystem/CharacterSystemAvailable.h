// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterSystemAvailable.generated.h"

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
	float GetAttributeCurrentValue(EAttributeCode InAttributeCode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetAttributeMaxValue(EAttributeCode InAttributeCode);

};
