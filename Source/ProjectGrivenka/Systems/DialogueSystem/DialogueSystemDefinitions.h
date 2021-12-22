// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueSystemDefinitions.generated.h"

USTRUCT(BlueprintType)
struct FDialogueData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* DialogueAnim;
};

USTRUCT(BlueprintType)
struct FReplyData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ReplyId;
};


USTRUCT(BlueprintType)
struct FDialogueSystemContext
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Actor;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class AAIController* AIController;
};