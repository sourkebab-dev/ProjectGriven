// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateRandomChance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGenerateRandomChance::UGenerateRandomChance() {
	this->NodeName = TEXT("Generate Random Chance");
	//this->bCreateNodeInstance = true;
	//this->bNotifyTick = 1;
}

EBTNodeResult::Type UGenerateRandomChance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;
	BlackboardComp->SetValueAsBool(this->GetSelectedBlackboardKey(), UKismetMathLibrary::RandomBoolWithWeight(this->Chance));
	return EBTNodeResult::Succeeded;
}