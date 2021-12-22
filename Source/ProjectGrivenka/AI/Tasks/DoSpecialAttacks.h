// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DoSpecialAttacks.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UDoSpecialAttacks : public UBTTaskNode
{
	GENERATED_BODY()
	float CurrentPooledTime;
	float MontageLength;

public:
	UPROPERTY(EditAnywhere, Category = RelativeDirection)
	class UAnimMontage* AttackMontage;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UDoSpecialAttacks();

};
