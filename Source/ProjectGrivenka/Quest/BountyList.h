// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestDefinitions.h"
#include "BountyList.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UBountyList : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBountyContract> BountyList;
};
