// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "CharacterPrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UCharacterPrefab : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
	FPersistedCharacterData CharacterData;

};
