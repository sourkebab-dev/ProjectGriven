// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateRandomTaskId.h"
#include "BehaviorTree/BlackboardComponent.h"

UGenerateRandomTaskId::UGenerateRandomTaskId() {
	this->NodeName = "Randomize Task id";
	this->bCallTickOnSearchStart = true;
	//this->bCreateNodeInstance = true;
}

void UGenerateRandomTaskId::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(this->GetSelectedBlackboardKey(), FMath::RandRange(this->Min, this->Max));
}