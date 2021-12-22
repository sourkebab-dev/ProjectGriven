// Fill out your copyright notice in the Description page of Project Settings.


#include "DoHeavyAttack.h"
#include "ProjectGrivenka/AI/BaseAIController.h"

UDoHeavyAttack::UDoHeavyAttack() {
	this->NodeName = TEXT("Do Heavy Attack");
	this->bCreateNodeInstance = true;
	this->bNotifyTick = 1;
}

EBTNodeResult::Type UDoHeavyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	this->CurrentPooledTime = 0.0;
	this->RealWaitTime = 0.0;

	if (!OwnerComp.GetAIOwner()) return EBTNodeResult::Failed;
	ABaseAIController* Ctrl = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (Ctrl) {
		this->RealWaitTime = FMath::RandRange(this->ChargeWaitTime - this->Deviation, this->ChargeWaitTime + this->Deviation);
		Ctrl->HeavyAttackCharge();
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UDoHeavyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	this->CurrentPooledTime += DeltaSeconds;
	if (this->CurrentPooledTime >= this->RealWaitTime) {
		if (!OwnerComp.GetAIOwner()) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		ABaseAIController* Ctrl = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
		if (Ctrl) {
			Ctrl->HeavyAttackRelease();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}