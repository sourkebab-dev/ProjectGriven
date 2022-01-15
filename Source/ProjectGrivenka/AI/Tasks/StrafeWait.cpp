// Fill out your copyright notice in the Description page of Project Settings.


#include "StrafeWait.h"
#include "AIController.h"
#include "ProjectGrivenka/VectorMathLib.h"

UStrafeWait::UStrafeWait() {
	this->NodeName = TEXT("Strafe Wait");
	this->bCreateNodeInstance = true;
	this->bNotifyTick = 1;
}

EBTNodeResult::Type UStrafeWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	this->CurrentPooledTime = 0.0;
	this->CurrentPooledSegment = 0.0;
	this->CurrentStrafeDir = this->StrafeStartRelativeDirection;
	return EBTNodeResult::InProgress;
}

void UStrafeWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.GetAIOwner() || !OwnerComp.GetAIOwner()->GetPawn() ) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	float Degrees = UVectorMathLib::DegreesBetweenVectors(FVector::ForwardVector, Pawn->GetActorForwardVector());
	Pawn->AddMovementInput(this->CurrentStrafeDir.RotateAngleAxis(Degrees, FVector::UpVector) * this->StrafeSpeed * DeltaSeconds);

	this->CurrentPooledSegment += DeltaSeconds;
	this->CurrentPooledTime += DeltaSeconds;
	if (this->CurrentPooledTime >= this->TotalWaitTime) FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	else if (this->CurrentPooledSegment >= this->TotalWaitTime / this->NumOfDirections) {
		this->CurrentPooledSegment = 0.0;
		this->CurrentPooledTime -= fmod(this->CurrentPooledTime, this->TotalWaitTime / this->NumOfDirections);
		this->CurrentStrafeDir = this->CurrentStrafeDir.RotateAngleAxis(this->CycleAngleDegrees, FVector::UpVector);
	}
	
}