// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipmentDefinitions.h"
#include "WeaponPrefabs.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UWeaponPrefabs : public UDataAsset
{
	GENERATED_BODY()
	
public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponInfo WeaponInfo;
};
