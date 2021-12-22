// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "ShowReply.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UShowReply : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Dialogue)
	FDialogueData DialogueQuery;

	UPROPERTY(EditAnywhere, Category = Dialogue)
	TArray<FReplyData> ReplyData;

	UShowReply();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION(BlueprintCallable)
	virtual void ReplyChosen(FName ReplyId);
	
};
