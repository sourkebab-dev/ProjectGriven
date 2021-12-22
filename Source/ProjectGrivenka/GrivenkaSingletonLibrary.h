// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GrivenkaDataSingleton.h"
#include "GrivenkaSingletonLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UGrivenkaSingletonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Singleton Getter")
	static UGrivenkaDataSingleton* GetGrivenkaData();
};
