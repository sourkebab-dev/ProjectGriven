// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EffectPrefab.h"
#include "EffectPrefabList.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UEffectPrefabList : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UEffectPrefab*> EffectAssets;
};
