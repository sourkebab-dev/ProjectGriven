// Fill out your copyright notice in the Description page of Project Settings.


#include "SlideState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

//sponge: need to add impact on characters
bool USlideState::StateValidation_Implementation()
{
	return Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)
		&& Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp)
		&& Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp)->IsMovingOnGround();
}

void USlideState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{

}

void USlideState::Tick_Implementation(float DeltaTime)
{
	FFindFloorResult FloorResult;
	this->CharMove->FindFloor(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), FloorResult, true);
	FVector FloorNormal = FloorResult.HitResult.Normal;
	float InfluenceMultiplier = 1 - FVector::DotProduct(FVector::UpVector, FloorNormal);
	FVector InfluenceDir = FVector::CrossProduct(FVector::CrossProduct(FVector::UpVector, FloorNormal), FloorNormal);
	this->CharMove->AddForce(InfluenceDir * 550000 * InfluenceMultiplier);

	if (this->CharMove->Velocity.Size() >= TERMINALVELOCITY) {
		FVector NormalizedVelocity = this->CharMove->Velocity;
		NormalizedVelocity.Normalize();
		this->CharMove->Velocity = NormalizedVelocity * TERMINALVELOCITY;
	}
	else if (this->CharMove->Velocity.Size() <= MINSLIDEVELOCITY) {
		this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("Recover");
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}


	if (!this->IsLaunched) return;

	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutResults;
	ActorsToIgnore.Add(this->StatesComp->CompContext->CharacterActor);

	//sponge: Tracechannel probably wrong
	if (UKismetSystemLibrary::SphereTraceMulti(this->GetWorld(), this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->StatesComp->CompContext->CharacterActor->GetActorLocation(), 200.0f,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutResults, true)) {
		for (int i = 0; i < OutResults.Num(); i++) {
			AActor* HitActor = OutResults[i].Actor.Get();
			if (!HitActor || this->HitList.Contains(HitActor) || !IAIContextSystemAvailable::Execute_CheckHostility(this->StatesComp->CompContext->CharacterActor, HitActor)) continue;
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(HitActor);
			if (!InstigatorCtx->EventBus) return;
			this->HitList.Add(HitActor);

			FDamageInfo StaggerDamage;
			StaggerDamage.ImpactType = EDamageImpactType::DI_HIGH;
			StaggerDamage.HitType = EDamageHitType::PUSH;
			StaggerDamage.DamageDirection = EHitDirectionType::FRONT;
			InstigatorCtx->EventBus->HitDelegate.Broadcast(this->StatesComp->CompContext->CharacterActor, StaggerDamage);
		}
	}

}

void USlideState::OnSlidePush(EAnimEvt AnimEvt)
{
	if (AnimEvt != EAnimEvt::BLOCK_PUSH) return;
	this->IsLaunched = true;
}

void USlideState::OnStateEnter_Implementation()
{
	this->CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	this->CharMove->GroundFriction = 0;
	this->CharMove->BrakingDecelerationWalking = 1000.0f;
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->SlideMontage);
	FVector LaunchVelocity = this->StatesComp->CompContext->CharacterActor->GetActorForwardVector() * 1500.0f;
	this->CharMove->AddImpulse(LaunchVelocity, true);
	this->StatesComp->CompContext->EventBus->AnimDelegate.AddDynamic(this, &USlideState::OnSlidePush);

}

void USlideState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.25);
	this->CharMove->GroundFriction = DEFAULTGROUNDFRICTION;
	this->CharMove->BrakingDecelerationWalking = DEFAULTBRAKINGDECELERATION;
	this->StatesComp->CompContext->EventBus->AnimDelegate.RemoveDynamic(this, &USlideState::OnSlidePush);
}
