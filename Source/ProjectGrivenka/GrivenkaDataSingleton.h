// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "ProjectGrivenka/Systems/TraitSystem/TraitListPrefab.h"
#include "ProjectGrivenka/Systems/CharacterSystem/EffectPrefabList.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabList.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "ProjectGrivenka/Equipments/SmithingRecipeTreePrefab.h"
#include "ProjectGrivenka/Items/ItemPrefabList.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/Quest/BountyList.h"
#include "GrivenkaDataSingleton.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTGRIVENKA_API UGrivenkaDataSingleton : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingletonData")
	UDataTable* SKMeshData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingletonData")
	UDataTable* MaterialData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingletonData")
	UCurveFloat* CommonDodgeCurveFloat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UTraitListPrefab* TraitPrefab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UWeaponPrefabList* WeaponPrefabs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UEffectPrefabList* EffectPrefab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UItemPrefabList* ItemPrefab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UItemPrefabList* MaterialMasterData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	TMap<TEnumAsByte<EEquipmentTree>, USmithingRecipeTreePrefab*> SmithWeaponRecipe;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	UBountyList* BountyList;

public:
	// usually for hairs and shit
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Getters")
	FBodyInfo GetBodyInfo(FName BodyId);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Getters")
	FBodyTypeData GetBodyTypeData(int TypeId);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Getters")
	FWearableInfo GetWearableInfo(FName WearableId);

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Getters")
	FSkinColor GetSkinColor(FName SkinColorId);

};