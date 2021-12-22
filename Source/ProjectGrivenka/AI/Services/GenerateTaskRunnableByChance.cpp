// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateTaskRunnableByChance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGenerateTaskRunnableByChance::UGenerateTaskRunnableByChance() {
	this->NodeName = "True By Chance";
	this->bCallTickOnSearchStart = true;
	//this->bCreateNodeInstance = true;
}

void UGenerateTaskRunnableByChance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(this->GetSelectedBlackboardKey(), UKismetMathLibrary::RandomBoolWithWeight(this->Chance));
}
