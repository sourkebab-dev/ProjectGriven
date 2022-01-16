// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitComboOpen.h"
#include "AIController.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"

UWaitComboOpen::UWaitComboOpen() {
	this->NodeName = TEXT("Wait Combo Open");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UWaitComboOpen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!AIPawn->Implements<UContextAvailable>()) return EBTNodeResult::Failed;
	IContextAvailable::Execute_GetContext(AIPawn, this->CharCtx);
	if (!this->CharCtx.EventBus) return EBTNodeResult::Failed;
	this->CharCtx.EventBus->AnimDelegate.RemoveDynamic(this, &UWaitComboOpen::OnOpenComboTriggered);
	this->CharCtx.EventBus->AnimDelegate.AddDynamic(this, &UWaitComboOpen::OnOpenComboTriggered);
	this->GetWorld()->GetTimerManager().SetTimer(this->ForceExitTimer, this, &UWaitComboOpen::OnForceExitTimer, this->DefaultExitTime);
	return EBTNodeResult::InProgress;
}

void UWaitComboOpen::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (AIPawn) {
		AIPawn->GetWorldTimerManager().ClearTimer(this->ForceExitTimer);
		AIPawn->GetWorldTimerManager().ClearTimer(this->PaddingTimer);
	}
	if (this->CharCtx.EventBus) {
		this->CharCtx.EventBus->AnimDelegate.RemoveDynamic(this, &UWaitComboOpen::OnOpenComboTriggered);
	}
}

void UWaitComboOpen::OnOpenComboTriggered(EAnimEvt EventType)
{
	if (EventType == EAnimEvt::ACTIVATE_COMBO) {
		this->GetWorld()->GetTimerManager().SetTimer(this->PaddingTimer, this, &UWaitComboOpen::OnWaitFinished, this->TargetPaddingTime);
	}
}

void UWaitComboOpen::OnForceExitTimer() {
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(this->GetOuter());
	this->FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}

void UWaitComboOpen::OnWaitFinished() {
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(this->GetOuter());
	this->FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}