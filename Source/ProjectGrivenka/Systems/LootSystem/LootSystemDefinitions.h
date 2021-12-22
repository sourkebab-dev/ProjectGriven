// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "LootSystemDefinitions.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UItemPrefab* Item;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LootWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool OnceTaken;
};

USTRUCT(BlueprintType)
struct FActionTagToLoot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool AlwaysGiven;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLootItem LootGiven;
};