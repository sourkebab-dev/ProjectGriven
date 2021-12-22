// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterInventory.h"
#include "UObject/Interface.h"
#include "CharacterInventoryAvailable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCharacterInventoryAvailable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API ICharacterInventoryAvailable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UCharacterInventory* GetCharacterInventoryComp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int GetSelectedItemCount();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class ABaseItem* GetCommitedItem();

};
