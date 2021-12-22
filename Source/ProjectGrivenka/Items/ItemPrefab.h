// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDefinitions.h"
#include "ItemPrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UItemPrefab : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemInfo ItemInfo;
};
