// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"


void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
	this->BTStart();
	this->BlackboardComp = this->GetBlackboardComponent();
	//sponge: bb syncing might need to change ???
	this->BlackboardComp->SetValueAsEnum("AIState", this->DefaultAIState);

}


void ABaseAIController::OnPossess(APawn* PossesedPawn)
{
	Super::OnPossess(PossesedPawn);
	this->GetWorldTimerManager().SetTimer(this->SightRefreshTimer, this, &ABaseAIController::SightRefresh, 1, true);

	//Note: This is for char switching, (actor already spawned and controlled by player, but later switched as npc)
	this->OnContextSetup();
}

void ABaseAIController::OnUnPossess()
{
	this->AggroRefreshTimer.Invalidate();
	this->SightRefreshTimer.Invalidate();
	this->AggroTarget = nullptr;
	this->AggroMap.Empty();
	if (this->ActorCtx.EventBus) {
		this->ActorCtx.EventBus->DamagedDelegate.RemoveAll(this);
	}
}

//sponge: might be a better way to catch the context initialization
//Right now it's currently being called in each BP Class after context init
void ABaseAIController::OnContextSetup()
{
	if (this->GetPawn() && this->GetPawn()->Implements<UContextAvailable>()) {
		IContextAvailable::Execute_GetContext(this->GetPawn(), this->ActorCtx);
		this->ActorCtx.AIController = this;
	}

	if (!this->ActorCtx.EventBus) return;

	this->ActorCtx.EventBus->DamagedDelegate.AddDynamic(this, &ABaseAIController::OnHit);
}

void ABaseAIController::SightRefresh()
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutResults;
	ActorsToIgnore.Add(this->GetPawn());

	if (UKismetSystemLibrary::SphereTraceMulti(this->GetWorld(), this->GetPawn()->GetActorLocation(), this->GetPawn()->GetActorLocation(),  500.0f,  
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutResults, true)) {
		for (int i = 0; i < OutResults.Num(); i++) {
			AActor* HitActor = OutResults[i].Actor.Get();
			if (HitActor) {
				FVector ToTarget = HitActor->GetActorLocation() - this->GetPawn()->GetActorLocation();
				ToTarget.Normalize();
				FVector ForwardVec = this->GetPawn()->GetActorForwardVector();
				ForwardVec.Normalize();

				if (FVector::DotProduct(ForwardVec, ToTarget) > 0.7) {
					UKismetSystemLibrary::DrawDebugSphere(this->GetWorld(), HitActor->GetActorLocation(), 50.0f, 12, FLinearColor::Blue, 5.0f);
					this->OnActorSeen(HitActor);
				}

			}
		}
	}

}

void ABaseAIController::AggroRefresh()
{
	if (this->BlackboardComp->GetValueAsEnum("AIState") != EAIStateType::COMBAT) return;
	TArray<AActor*> Outkeys;
	float MaxAggroPoint = 0.0f;
	this->AggroMap.GetKeys(Outkeys);
	AActor* TempAggroPoint = nullptr;
	for (int i = 0; i < Outkeys.Num(); i++) {
		if (this->AggroMap[Outkeys[i]] >= MaxAggroPoint) {
			MaxAggroPoint = this->AggroMap[Outkeys[i]];
			TempAggroPoint = Outkeys[i];
		}
		this->AggroMap[Outkeys[i]] = 0;
	}
	this->SetAggroTarget(TempAggroPoint);
}

void ABaseAIController::OnActorSeen(AActor* SeenActor)
{
	if (CheckHostility(SeenActor) && !this->AggroMap.Find(SeenActor)) {
		if (this->AggroMap.Num() == 0) this->SetAggroTarget(SeenActor);
		this->AddAggroActor(SeenActor , 0);
		this->ChangeAIState(EAIStateType::COMBAT);
	}
}

bool ABaseAIController::CheckHostility(AActor* HostilityToCheck)
{
	APawn* CheckPawn = Cast<APawn>(HostilityToCheck);
	if (!CheckPawn) return false;

	ABaseAIController* PawnController = Cast<ABaseAIController>(CheckPawn->GetController());
	EHostilityType ToCheckType;

	if (!PawnController && !CheckPawn->GetController()->IsPlayerController()) return false;
	else if (PawnController) ToCheckType = PawnController->HostilityType;
	else ToCheckType = EHostilityType::ALLY;

	switch (this->HostilityType)
	{
		case EHostilityType::ALLY:
			return ToCheckType == EHostilityType::WILD || ToCheckType == EHostilityType::HOSTILE;
		case EHostilityType::HOSTILE:
			return ToCheckType == EHostilityType::WILD || ToCheckType == EHostilityType::ALLY;
		case EHostilityType::WILD:
			return ToCheckType == EHostilityType::HOSTILE || ToCheckType == EHostilityType::NEUTRAL || ToCheckType == EHostilityType::ALLY;
		case EHostilityType::NEUTRAL:
			return false;
		default:
			return false;
	}
	
	
}

void ABaseAIController::ChangeAIState(TEnumAsByte<EAIStateType> NewAIState)
{
	GLog->Log("ChangeONrun?");

	if (this->BlackboardComp->GetValueAsEnum("AIState") == NewAIState) return;

	this->BlackboardComp->SetValueAsEnum("AIState", NewAIState);

	if (NewAIState == EAIStateType::COMBAT) {
		this->GetWorldTimerManager().SetTimer(this->AggroRefreshTimer, this, &ABaseAIController::AggroRefresh, 10, true, 0);
	}
	else {
		this->AggroRefreshTimer.Invalidate();
		this->AggroMap.Empty();
		this->SetAggroTarget(nullptr);
	}


}

void ABaseAIController::OnHit(AActor* DamageInstigator, FDamageInfo InDamageInfo)
{
	GLog->Log("hitai");
	switch (this->BlackboardComp->GetValueAsEnum("AIState"))
	{
	case EAIStateType::COMBAT:
		GLog->Log("CombatState");
		break;
	case EAIStateType::IDLE:
		GLog->Log("Idle");
		break;
	default:
		break;
	}
	//sponge: need to find how to generate aggro value
	if (this->BlackboardComp->GetValueAsEnum("AIState") != EAIStateType::COMBAT) {
		GLog->Log("??");
		this->SetAggroTarget(DamageInstigator);
		this->ChangeAIState(EAIStateType::COMBAT);
	}

	this->AddAggroActor(DamageInstigator, InDamageInfo.RawPhysicalDamage);
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
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionDodge, IE_Pressed);
}

void ABaseAIController::Attack() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
}

void ABaseAIController::HeavyAttackCharge() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
}

void ABaseAIController::HeavyAttackRelease() {
	if (!this->ActorCtx.EventBus) return;
	this->ActorCtx.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
}

void ABaseAIController::SetBBAggroTarget(AActor* NewAggroTarget)
{
	this->BlackboardComp->SetValueAsObject("TargetAggro", NewAggroTarget);
}

void ABaseAIController::SetBBCharacterState(FGameplayTagContainer NewState) {
	//this->BlackboardComp->SetValueAsEnum("CurrentState", NewState);
}



