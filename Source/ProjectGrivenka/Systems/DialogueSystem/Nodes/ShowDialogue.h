// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "ShowDialogue.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UShowDialogue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

		//adds a wait a few time after combo is open before hitting attack to create organic attack movement
	UPROPERTY(EditAnywhere, Category = DialogueData)
	TArray<FDialogueData> DialogueData;

	int CurrDialogueIndex = 0;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void DisplayLine();

	UFUNCTION(BlueprintCallable)
	virtual void ProceedDialogue();

	UShowDialogue();
};
