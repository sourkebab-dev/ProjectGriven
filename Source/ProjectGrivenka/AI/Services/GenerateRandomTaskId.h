// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "GenerateRandomTaskId.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UGenerateRandomTaskId : public UBTService_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Deviation)
	int Min;

	UPROPERTY(EditAnywhere, Category = Deviation)
	int Max;

	UGenerateRandomTaskId();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
