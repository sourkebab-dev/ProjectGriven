// Fill out your copyright notice in the Description page of Project Settings.


#include "DoAttack.h"
#include "Animation/AnimMontage.h"
#include "ProjectGrivenka/AI/BaseAIController.h"

UDoAttack::UDoAttack() {
	this->NodeName = TEXT("Do Attack");
}

EBTNodeResult::Type UDoAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!OwnerComp.GetAIOwner()) return EBTNodeResult::Failed;
	ABaseAIController* Ctrl = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if(Ctrl){
		Ctrl->Attack();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

