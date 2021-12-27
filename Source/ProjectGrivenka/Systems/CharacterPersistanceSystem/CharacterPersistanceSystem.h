// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/PersistedData/Persistable.h"
#include "CharacterPersistanceSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIVENKA_API UCharacterPersistanceSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void LoadData(FPersistedCharacterData InCharacterData);

	UFUNCTION(BlueprintCallable)
	void SaveData(FPersistedCharacterData& OutCharacterData);
};
