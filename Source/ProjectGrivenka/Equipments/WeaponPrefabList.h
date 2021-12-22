// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponPrefabs.h"
#include "WeaponPrefabList.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UWeaponPrefabList : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UWeaponPrefabs*> WeaponAssets;

};
