// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "InviteToParty.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UInviteToParty : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UInviteToParty();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
