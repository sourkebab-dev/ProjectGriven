// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/Character/CharacterPrefab.h"
#include "QuestDefinitions.generated.h"

USTRUCT(BlueprintType)
struct FBountyContract
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LevelName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UCharacterPrefab*> BountyTargets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BountyQuestName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BountyQuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int BountyReward;

    //Maybe add type? Alive, Dead or Alive
};
