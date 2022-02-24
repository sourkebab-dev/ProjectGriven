// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TraitSystemDefinitions.generated.h"


USTRUCT(BlueprintType)
struct FAbilityInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class UBaseState> GrantedAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilityId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText AbilityDescriptions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* AbilityIcon;

};

USTRUCT(BlueprintType)
struct FTraitInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer TraitTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TraitId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TraitName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText TraitDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* TraitIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<class UEffectPrefab*> AttributeTraits;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<class UAbilityPrefab*> AbilityTraits;
};