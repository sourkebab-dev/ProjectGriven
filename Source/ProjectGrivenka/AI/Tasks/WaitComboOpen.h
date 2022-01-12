// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "WaitComboOpen.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UWaitComboOpen : public UBTTask_BlackboardBase
{

	GENERATED_BODY()
	
	//adds a wait a few time after combo is open before hitting attack to create organic attack movement
	UPROPERTY(EditAnywhere, Category = Wait)
	float TargetPaddingTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = Wait)
	float DefaultExitTime = 3.0f;

	FTimerHandle ForceExitTimer;
	FTimerHandle PaddingTimer;
	FCharacterContext CharCtx;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UFUNCTION()
	void OnOpenComboTriggered(EAnimEvt EventType);

	UFUNCTION()
	void OnForceExitTimer();

	UFUNCTION()
	void OnWaitFinished();

	UWaitComboOpen();

};
