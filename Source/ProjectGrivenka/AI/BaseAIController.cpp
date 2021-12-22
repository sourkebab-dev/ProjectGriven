// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"


void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
	this->BTStart();
	this->BlackboardComp = this->GetBlackboardComponent();
}

void ABaseAIController::OnPossess(APawn* PossesedPawn)
{
	Super::OnPossess(PossesedPawn);
	if (this->GetPawn() && this->GetPawn()->Implements<UContextAvailable>()) {
		IContextAvailable::Execute_GetContext(this->GetPawn(), this->ActorCtx);
		this->ActorCtx.AIController = this;
	}
}


void ABaseAIController::BTStart() {
	if (this->BTree) {
		this->RunBehaviorTree(this->BTree);
	}
}


void ABaseAIController::Dodge() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionDodge, IE_Pressed);
}

void ABaseAIController::Attack() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
}

void ABaseAIController::HeavyAttackCharge() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionHeavyAttack, IE_Pressed);
}

void ABaseAIController::HeavyAttackRelease() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionHeavyAttack, IE_Released);
}

void ABaseAIController::SetBBAggroTarget(AActor* NewAggroTarget)
{
	this->BlackboardComp->SetValueAsObject("TargetAggro", NewAggroTarget);
}

void ABaseAIController::SetBBCharacterState(FGameplayTagContainer NewState) {
	//this->BlackboardComp->SetValueAsEnum("CurrentState", NewState);
}



