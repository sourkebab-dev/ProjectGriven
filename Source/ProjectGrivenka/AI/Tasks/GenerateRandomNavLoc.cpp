// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateRandomNavLoc.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

//sponge: might need to implement eqs for this
EBTNodeResult::Type UGenerateRandomNavLoc::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp || !OwnerComp.GetAIOwner()) return EBTNodeResult::Failed;
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	FNavLocation RandPoint;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(this->GetWorld());
	if (NavSys->GetRandomReachablePointInRadius(AIPawn->GetActorLocation(), 700.0f, RandPoint)) {
		BlackboardComp->SetValueAsVector(this->GetSelectedBlackboardKey(), RandPoint.Location);
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
}