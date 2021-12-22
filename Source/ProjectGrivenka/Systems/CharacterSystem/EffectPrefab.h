// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterSystemDefinitions.h"
#include "EffectPrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UEffectPrefab : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffectInfo EffectInfo;
};
