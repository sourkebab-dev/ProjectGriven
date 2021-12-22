// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "Persistable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPersistable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API IPersistable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LoadData(FPersisted_CharacterCompleteData CharacterData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SaveData();

};
