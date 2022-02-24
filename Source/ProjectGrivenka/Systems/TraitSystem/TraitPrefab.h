// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TraitSystemDefinitions.h"
#include "TraitPrefab.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UTraitPrefab : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTraitInfo TraitInfo;

};
