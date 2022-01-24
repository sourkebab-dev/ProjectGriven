// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "GenerateDefaultDialogueChoice.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UGenerateDefaultDialogueChoice : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Dialogue)
	FDialogueData DialogueQuery;

	UGenerateDefaultDialogueChoice();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION(BlueprintCallable)
	virtual void ReplyChosen(FName ReplyId);
};
