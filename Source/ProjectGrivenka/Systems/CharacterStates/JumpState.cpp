// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpState.h"
#include "GameFramework/Character.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"

bool UJumpState::StateValidation_Implementation()
{
	return Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)
		&& Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp)
		&& Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp)->IsMovingOnGround();
}

void UJumpState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionAttack && EventType == IE_Pressed ) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Attack.Air"), EActionList::ActionNone, IE_Released);
	}
}

void UJumpState::Tick_Implementation(float DeltaTime)
{
	if (!this->CharMove->IsFalling()) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}

	if (this->CharMove->Velocity.Z < 0) {
		this->BlockedTag.RemoveAllTags();
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.InAir"), EActionList::ActionNone, IE_Released);
		return;
	}
}

void UJumpState::OnStateEnter_Implementation()
{
	this->CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	this->StatesComp->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UJumpState::OnJumpLaunchedTriggered);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->JumpMontage);
}


void UJumpState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->EventBus->AnimDelegate.RemoveDynamic(this, &UJumpState::OnJumpLaunchedTriggered);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->JumpMontage, 1.0f);

}

void UJumpState::OnJumpLaunchedTriggered(EAnimEvt EventType)
{
	//match anim play rate to launch duration
	this->CharMove->GravityScale = 2.0f;
	auto CharacterActor = Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor);
	this->ToApexDuration = this->JumpZVelocity / this->CharMove->GetGravityZ();
	float LaunchAnimDuration = this->JumpMontage->GetSectionLength(this->JumpMontage->GetSectionIndex("Launch"));
	float NewPlayRate = LaunchAnimDuration / this->ToApexDuration;
	this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->JumpMontage, NewPlayRate);
	CharacterActor->LaunchCharacter(FVector(0.0f, 0.0f, this->JumpZVelocity), false, true);

}
