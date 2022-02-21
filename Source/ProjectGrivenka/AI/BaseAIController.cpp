// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"

ABaseAIController::ABaseAIController() : AAIController() {
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
	this->BTStart();
	this->BlackboardComp = this->GetBlackboardComponent();
}

//sponge: Might need to move this ai rotation func to somewhere else
void ABaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!this->AggroTarget || !this->ActorCtx) return;
	UVectorMathLib::RotateActorToTargetVector(this->ActorCtx->CharacterActor, this->AggroTarget->GetActorLocation(), this->RotationRate, DeltaTime);
}

void ABaseAIController::OnPossess(APawn* PossesedPawn)
{
	Super::OnPossess(PossesedPawn);
	this->GetWorldTimerManager().SetTimer(this->SightRefreshTimer, this, &ABaseAIController::SightRefresh, 0.5, true);

	//Note: This is for char switching, (actor already spawned and controlled by player, but later switched as npc)
	this->OnContextSetup();
}

void ABaseAIController::OnUnPossess()
{
	this->ActorCtx->EventBus->HitDelegate.RemoveDynamic(this, &ABaseAIController::OnHit);
	this->ActorCtx->EventBus->AnimDelegate.RemoveDynamic(this, &ABaseAIController::SetRotationRate);
	this->ActorCtx->CharacterActor->GetWorldTimerManager().ClearTimer(this->AggroRefreshTimer);
	this->ActorCtx->CharacterActor->GetWorldTimerManager().ClearTimer(this->SightRefreshTimer);

	this->AggroTarget = nullptr;
	this->AggroMap.Empty();
	this->ActorCtx->EventBus->HitDelegate.RemoveAll(this);
	Super::OnUnPossess();
}

//sponge: might be a better way to catch the context initialization
//Right now it's currently being called in each BP Class after context init
void ABaseAIController::OnContextSetup()
{
	this->ActorCtx = IContextAvailable::Execute_GetContext(this->GetPawn());
	if (!this->ActorCtx) return;
	this->ActorCtx->Controller = this;

	if (!this->ActorCtx || !this->ActorCtx->EventBus) return;
	this->ActorCtx->EventBus->HitDelegate.AddDynamic(this, &ABaseAIController::OnHit);
	this->ActorCtx->EventBus->AnimDelegate.AddDynamic(this, &ABaseAIController::SetRotationRate);
}

void ABaseAIController::SetRotationRate(EAnimEvt InAnimEvt)
{
	switch (InAnimEvt)
	{
		case FULL_ROTATION:
			this->RotationRate = 20;
			break;
		case SLOW_ROTATION:
			this->RotationRate = 8;
			break;
		case OFF_ROTATION:
			this->RotationRate = 0.01;
			break;
		default:
			break;
	}
}

void ABaseAIController::SightRefresh()
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutResults;
	ActorsToIgnore.Add(this->GetPawn());

	if (UKismetSystemLibrary::SphereTraceMulti(this->GetWorld(), this->GetPawn()->GetActorLocation(), this->GetPawn()->GetActorLocation(),  500.0f,  
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::None, OutResults, true)) {
		for (int i = 0; i < OutResults.Num(); i++) {
			AActor* HitActor = OutResults[i].Actor.Get();
			if (HitActor) {
				FVector ToTarget = HitActor->GetActorLocation() - this->GetPawn()->GetActorLocation();
				ToTarget.Normalize();
				FVector ForwardVec = this->GetPawn()->GetActorForwardVector();
				ForwardVec.Normalize();

				if (FVector::DotProduct(ForwardVec, ToTarget) > 0.1) {
					//UKismetSystemLibrary::DrawDebugSphere(this->GetWorld(), HitActor->GetActorLocation(), 50.0f, 12, FLinearColor::Blue, 5.0f);
					this->OnActorSeen(HitActor);
				}

			}
		}
	}



	if (this->AggroMap.Num() == 0) return;
	TArray<AActor*> Outkeys;
	TArray<AActor*> ToRemove;
	this->AggroMap.GetKeys(Outkeys);
	for (int i = 0; i < Outkeys.Num(); i++) {
		auto Ctx = IContextAvailable::Execute_GetContext(Outkeys[i]);
		if (Ctx && Ctx->CombatModule.IsDead) ToRemove.Add(Outkeys[i]);
	}
	for (int i = 0; i < ToRemove.Num(); i++) {
		this->AggroMap.Remove(ToRemove[i]);
	}
	if (this->AggroMap.Num() == 0) this->ChangeAIState(EAIStateType::IDLE);
}

void ABaseAIController::AggroRefresh()
{
	if (this->BlackboardComp->GetValueAsEnum("AIState") != EAIStateType::COMBAT) return;
	TArray<AActor*> Outkeys;
	float MaxAggroPoint = 0.0f;
	float ClosestDist = 99999.0f;
	this->AggroMap.GetKeys(Outkeys);
	AActor* TempAggroPoint = nullptr;
	AActor* ClosestAggro = nullptr;
	for (int i = 0; i < Outkeys.Num(); i++) {
		if (this->AggroMap[Outkeys[i]] >= MaxAggroPoint) {
			MaxAggroPoint = this->AggroMap[Outkeys[i]];
			TempAggroPoint = Outkeys[i];
		}

		float Dist = Outkeys[i]->GetDistanceTo(this->ActorCtx->CharacterActor);
		if (Dist < ClosestDist) {
			ClosestAggro = Outkeys[i];
			ClosestDist = Dist;
		}

		this->AggroMap[Outkeys[i]] = 0;


		//sponge: might be better using events/traces
	}

	this->SetAggroTarget(MaxAggroPoint == 0.0 ? ClosestAggro : TempAggroPoint);
}

void ABaseAIController::OnActorSeen(AActor* SeenActor)
{
	if (!this->ActorCtx->CharacterActor->Implements<UAIContextSystemAvailable>()) return;

	if (IAIContextSystemAvailable::Execute_CheckHostility(this->ActorCtx->CharacterActor, SeenActor) && !this->AggroMap.Find(SeenActor)) {
		if (this->AggroMap.Num() == 0) this->SetAggroTarget(SeenActor);
		this->AddAggroActor(SeenActor , 0);
		this->ChangeAIState(EAIStateType::COMBAT);

		//sponge: might be a gank source
		FCommandInfo Command;
		Command.CommandTargetActor = SeenActor;
		Command.CommandType = EAICommandType::ATTACK;
		IAIContextSystemAvailable::Execute_SignalCommandToArea(this->ActorCtx->CharacterActor, Command);
	}
}

void ABaseAIController::ChangeAIState(TEnumAsByte<EAIStateType> NewAIState)
{
	if (this->BlackboardComp->GetValueAsEnum("AIState") == NewAIState) return;

	this->BlackboardComp->SetValueAsEnum("AIState", NewAIState);

	if (NewAIState == EAIStateType::COMBAT) {
		this->GetWorldTimerManager().SetTimer(this->AggroRefreshTimer, this, &ABaseAIController::AggroRefresh, 5, true, 0);
	}
	else {
		this->ActorCtx->CharacterActor->GetWorldTimerManager().ClearTimer(this->AggroRefreshTimer);
		this->AggroMap.Empty();
		this->SetAggroTarget(nullptr);
	}


}

void ABaseAIController::OnHit(AActor* DamageInstigator, FDamageInfo InDamageInfo)
{
	if (InDamageInfo.HitType == UNINSTIGATED) return;
	//sponge: need to find how to generate aggro value
	if (this->BlackboardComp->GetValueAsEnum("AIState") != EAIStateType::COMBAT) {
		this->SetAggroTarget(DamageInstigator);
		this->ChangeAIState(EAIStateType::COMBAT);
	}

	this->AddAggroActor(DamageInstigator, InDamageInfo.RawPhysicalDamage);

	FCommandInfo Command;
	Command.CommandTargetActor = DamageInstigator;
	Command.CommandType = EAICommandType::ATTACK;
	IAIContextSystemAvailable::Execute_SignalCommandToArea(this->ActorCtx->CharacterActor, Command);
}


void ABaseAIController::AddAggroActor(AActor* AggroInstigator, float AggroPoints)
{
	if (this->AggroMap.Find(AggroInstigator)) {
		this->AggroMap[AggroInstigator] += AggroPoints;
	}
	else {
		this->AggroMap.Add(AggroInstigator, AggroPoints);
	}
}

void ABaseAIController::SetAggroTarget(AActor* AggroInstigator)
{
	this->AggroTarget = AggroInstigator;
	this->BlackboardComp->SetValueAsObject("TargetAggro", AggroInstigator);
}

void ABaseAIController::RemoveActorFromAggroList(AActor* AggroInstigator)
{
	this->AggroMap.Remove(AggroInstigator);
}

void ABaseAIController::BTStart() {
	if (this->BTree) {
		this->RunBehaviorTree(this->BTree);
	}
}


void ABaseAIController::Dodge() {
	if (!this->ActorCtx->EventBus) return;
	this->ActorCtx->EventBus->StateActionDelegate.Broadcast(EActionList::ActionDodge, IE_Pressed);
}

void ABaseAIController::Attack() {
	if (!this->ActorCtx->EventBus) return;
	this->ActorCtx->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
	this->ActorCtx->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
}

void ABaseAIController::HeavyAttackCharge() {
	if (!this->ActorCtx->EventBus) return;
	this->ActorCtx->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
}

void ABaseAIController::HeavyAttackRelease() {
	if (!this->ActorCtx->EventBus) return;
	this->ActorCtx->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
}

void ABaseAIController::SetBBDefendActor(AActor* DefendActor)
{
	this->BlackboardComp->SetValueAsObject("DefendActor", DefendActor);
}

void ABaseAIController::SetBBMovementLocation(FVector TargetLocation)
{
	this->BlackboardComp->SetValueAsVector("TargetMovement", TargetLocation);
}

void ABaseAIController::SendAIEvent(TEnumAsByte<EAIEvent> InAIEvent)
{
	//sponge: dunno if this is the best way to do it
	this->BlackboardComp->SetValueAsEnum("AIEvent", InAIEvent);
}


