// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentDefinitions.h"
#include "SmithingRecipePrefab.h"
#include "SmithingRecipeTreePrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API USmithingRecipeTreePrefab : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEquipmentType> EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EquipmentType == EEquipmentType::Weapon", EditConditionHides))
	TEnumAsByte<EWeaponTree> WeaponTreeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSmithingRecipeTreeItem> SmithingRecipeTree;
};
