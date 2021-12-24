// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/EffectPrefabList.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabList.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingletonData")
	FRotationRate CommonRotationRate = FRotationRate(0.01, 8, 20);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UWeaponPrefabList* WeaponPrefabs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UEffectPrefabList* EffectPrefab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PrefabData")
	UItemPrefabList* ItemPrefab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	UBountyList* BountyList;

};