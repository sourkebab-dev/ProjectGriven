// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorMathLib.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UVectorMathLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float DegreesBetweenVectors(FVector A, FVector B);
};
