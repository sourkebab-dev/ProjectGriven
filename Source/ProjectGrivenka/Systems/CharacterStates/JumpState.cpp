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

void UJumpState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
	if (Action == EActionList::ActionMoveForward || Action == EActionList::ActionMoveRight)
	{
		this->StatesComp->CompContext->MovementComp->AddInputVector(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir);
	}
}

void UJumpState::OnStateEnter_Implementation()
{
	this->CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	this->CharMove->bNotifyApex = true;
	this->CharAct = Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor); 
	this->CharAct->OnReachedJumpApex.AddDynamic(this, &UJumpState::OnJumpApexReached);
	this->StatesComp->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UJumpState::OnJumpLaunchedTriggered);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->JumpMontage);
}


void UJumpState::OnStateExit_Implementation()
{
	this->CharMove->GravityScale = 1.0f;
	this->CharAct->OnReachedJumpApex.RemoveDynamic(this, &UJumpState::OnJumpApexReached);
	this->StatesComp->CompContext->EventBus->AnimDelegate.RemoveDynamic(this, &UJumpState::OnJumpLaunchedTriggered);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->JumpMontage, 1.0f);

}

void UJumpState::OnJumpApexReached()
{
	this->StatesComp->BlockedTags.RemoveAllTags();
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.InAir"), EActionList::ActionNone, IE_Released);
}

void UJumpState::OnJumpLaunchedTriggered(EAnimEvt EventType)
{
	//match anim play rate to launch duration
	this->CharMove->GravityScale = this->GravityScaleModifier;
	auto CharacterActor = Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor);
	this->ToApexDuration = this->JumpZVelocity / FMath::Abs(this->CharMove->GetGravityZ());
	float LaunchAnimDuration = this->JumpMontage->GetSectionLength(this->JumpMontage->GetSectionIndex("Launch"));
	float NewPlayRate = LaunchAnimDuration / this->ToApexDuration;
	this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->JumpMontage, NewPlayRate);
	CharacterActor->LaunchCharacter(FVector(0.0f, 0.0f, this->JumpZVelocity), false, true);

}
