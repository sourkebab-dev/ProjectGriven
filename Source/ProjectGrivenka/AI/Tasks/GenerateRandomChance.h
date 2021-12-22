// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "GenerateRandomChance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UGenerateRandomChance : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UGenerateRandomChance();

	UPROPERTY(EditAnywhere, Category = Deviation)
	float Chance;

	
};
