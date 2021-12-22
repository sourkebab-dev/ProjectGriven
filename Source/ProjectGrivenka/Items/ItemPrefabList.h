// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemPrefab.h"
#include "ItemPrefabList.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UItemPrefabList : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UItemPrefab*> ItemAssets;
};
