// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_IsAtLocation.h"
#include "IsTargetInRange.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UIsTargetInRange : public UBTDecorator_IsAtLocation
{
	GENERATED_BODY()
	UIsTargetInRange();
};
