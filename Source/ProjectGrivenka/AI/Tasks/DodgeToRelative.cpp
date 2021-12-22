// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeToRelative.h"
#include "ProjectGrivenka/AI/BaseAIController.h"
#include "ProjectGrivenka/VectorMathLib.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"

UDodgeToRelative::UDodgeToRelative() {
	this->NodeName = TEXT("Dodge To Relative");
	this->bCreateNodeInstance = true;
	this->bNotifyTick = 1;
}

EBTNodeResult::Type UDodgeToRelative::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	this->CurrentPooledTime = 0.0;

	if (!OwnerComp.GetAIOwner()) return EBTNodeResult::Failed;
	APawn* ThisPawn = OwnerComp.GetAIOwner()->GetPawn();
	ABaseAIController* Ctrl = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	if (Ctrl && ThisPawn) {

		float Degrees = UVectorMathLib::DegreesBetweenVectors(FVector::ForwardVector , ThisPawn->GetActorForwardVector());

		if (ThisPawn->Implements<UContextAvailable>()) {
			FCharacterContext ActorCtx;
			IContextAvailable::Execute_GetContext(ThisPawn, ActorCtx);
			ActorCtx.Store->MovementModule.WorldSpaceTargetDir = this->RelativeDirection.RotateAngleAxis(Degrees, FVector::UpVector);
			Ctrl->Dodge();
			return EBTNodeResult::InProgress;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Invalid Character Instance"), *GetNameSafe(this));
		}
	}
	return EBTNodeResult::Failed;
}

void UDodgeToRelative::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	this->CurrentPooledTime += DeltaSeconds;
	if (this->CurrentPooledTime >= 0.45) {
		this->FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}