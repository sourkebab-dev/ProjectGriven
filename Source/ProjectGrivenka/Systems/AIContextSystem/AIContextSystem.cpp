// Fill out your copyright notice in the Description page of Project Settings.


#include "AIContextSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/AI/BaseAIController.h"


void UAIContextSystem::Init()
{
	Super::Init();
	if (!this->CompContext->EventBus) return;
	this->CompContext->EventBus->AICommandDelegate.AddDynamic(this, &UAIContextSystem::OnCommanded);

}

void UAIContextSystem::SignalCommandToActor(AActor* CommandReceiveActor, FCommandInfo CommandInfo) {
	if (!CommandReceiveActor || !CommandReceiveActor->Implements<UAIContextSystemAvailable>()) return;
	if (IAIContextSystemAvailable::Execute_GetHostilityType(CommandReceiveActor) != this->HostilityType) return;
	IAIContextSystemAvailable::Execute_OnCommanded(CommandReceiveActor, this->CompContext->CharacterActor, CommandInfo);
}

void UAIContextSystem::SignalCommandToArea(FCommandInfo CommandInfo)
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutResults;
	ActorsToIgnore.Add(this->CompContext->CharacterActor);

	if (UKismetSystemLibrary::SphereTraceMulti(this->GetWorld(), this->CompContext->CharacterActor->GetActorLocation(), this->CompContext->CharacterActor->GetActorLocation(), 500.0f,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutResults, true)) {
		for (int i = 0; i < OutResults.Num(); i++) {
			AActor* HitActor = OutResults[i].Actor.Get();
			this->SignalCommandToActor(HitActor, CommandInfo);
		}
	}
}

void UAIContextSystem::OnCommanded(AActor* CommandInstigator, FCommandInfo CommandInfo)
{
	auto AIC = Cast<ABaseAIController>(this->CompContext->Controller);
	if (!AIC) return;

	if (CommandInfo.CommandType == EAICommandType::ATTACK) {
		AIC->SetAggroTarget(CommandInfo.CommandTargetActor);
		AIC->AddAggroActor(CommandInfo.CommandTargetActor, 0);
		AIC->ChangeAIState(EAIStateType::COMBAT);
	}
	else if (CommandInfo.CommandType == EAICommandType::DEFEND) {

	}
	else if (CommandInfo.CommandType == EAICommandType::INTERACT) {

	}
	else if (CommandInfo.CommandType == EAICommandType::MOVETO) {
		AIC->ChangeAIState(EAIStateType::IDLE);
		AIC->SetBBMovementLocation(CommandInfo.CommandTargetLocation);
	}
}

bool UAIContextSystem::CheckHostility(AActor* ToCheckActor)
{
	if (!ToCheckActor->Implements<UAIContextSystemAvailable>()) return false;
	EHostilityType TypeB = IAIContextSystemAvailable::Execute_GetHostilityType(ToCheckActor);

	switch (this->HostilityType)
	{
	case EHostilityType::ALLY:
		return TypeB == EHostilityType::WILD || TypeB == EHostilityType::HOSTILE;
	case EHostilityType::HOSTILE:
		return TypeB == EHostilityType::WILD || TypeB == EHostilityType::ALLY;
	case EHostilityType::WILD:
		return TypeB == EHostilityType::HOSTILE || TypeB == EHostilityType::NEUTRAL || TypeB == EHostilityType::ALLY;
	case EHostilityType::NEUTRAL:
		return false;
	default:
		return false;
	}
}
