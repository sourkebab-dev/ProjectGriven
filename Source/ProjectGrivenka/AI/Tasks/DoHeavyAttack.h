// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DoHeavyAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UDoHeavyAttack : public UBTTaskNode
{
	GENERATED_BODY()
	float CurrentPooledTime;
	float RealWaitTime = 0.0;

public:
	UPROPERTY(EditAnywhere, Category = ChargeWait)
	float ChargeWaitTime = 1.5;
	UPROPERTY(EditAnywhere, Category = ChargeWait)
	float Deviation = 0.5;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UDoHeavyAttack();

};
