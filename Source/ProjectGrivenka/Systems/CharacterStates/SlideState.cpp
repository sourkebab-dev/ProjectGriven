// Fill out your copyright notice in the Description page of Project Settings.


#include "SlideState.h"
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


}

void USlideState::OnStateEnter_Implementation()
{
	this->CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	this->CharMove->GroundFriction = 0;
	this->CharMove->BrakingDecelerationWalking = 1000.0f;
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->SlideMontage);
	auto CharacterActor = Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor);
	FVector LaunchVelocity = this->StatesComp->CompContext->CharacterActor->GetActorForwardVector() * 1500.0f;
	CharacterActor->LaunchCharacter(LaunchVelocity, false, true);
}

void USlideState::OnStateExit_Implementation()
{
	this->CharMove->GroundFriction = DEFAULTGROUNDFRICTION;
	this->CharMove->BrakingDecelerationWalking = DEFAULTBRAKINGDECELERATION;
}
