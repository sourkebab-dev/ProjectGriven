// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EndDialogue.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UEndDialogue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UEndDialogue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
