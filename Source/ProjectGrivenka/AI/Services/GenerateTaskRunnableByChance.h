// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "GenerateTaskRunnableByChance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UGenerateTaskRunnableByChance : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
	UGenerateTaskRunnableByChance();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = Deviation)
	float Chance;

};
