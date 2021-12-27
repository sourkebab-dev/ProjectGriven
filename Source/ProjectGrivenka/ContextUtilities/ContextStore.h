// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ContextStore.generated.h"



USTRUCT(BlueprintType)
struct FContextMovement
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector WorldSpaceTargetDir;
};

USTRUCT(BlueprintType)
struct FContextInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid InstanceGuid;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName InstanceName;
};


UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UContextStore : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FContextMovement MovementModule;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FContextInfo InfoModule;
};
