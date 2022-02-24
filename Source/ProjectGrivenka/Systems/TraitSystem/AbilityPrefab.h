// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TraitSystemDefinitions.h"
#include "AbilityPrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UAbilityPrefab : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAbilityInfo AbilityInfo;

};
