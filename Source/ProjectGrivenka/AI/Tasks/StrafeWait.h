// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "StrafeWait.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UStrafeWait : public UBTTaskNode
{
	GENERATED_BODY()
	float CurrentPooledTime;
	float CurrentPooledSegment;
	FVector CurrentStrafeDir;

public:

	UPROPERTY(EditAnywhere, Category = Wait)
	float TotalWaitTime;

	UPROPERTY(EditAnywhere, Category = Direction)
	int NumOfDirections;

	UPROPERTY(EditAnywhere, Category = Direction)
	FVector StrafeStartRelativeDirection;

	UPROPERTY(EditAnywhere, Category = Direction)
	float CycleAngleDegrees;

	UPROPERTY(EditAnywhere, Category = Speed)
	float StrafeSpeed;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UStrafeWait();
};
