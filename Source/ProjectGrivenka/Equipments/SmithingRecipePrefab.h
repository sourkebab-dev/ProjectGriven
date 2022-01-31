// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentDefinitions.h"
#include "SmithingRecipePrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API USmithingRecipePrefab : public UDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    class UWeaponPrefabs* SmithingBaseWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    class UWeaponPrefabs* SmithingResultWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSmithingRecipeItem> Recipe;
};
