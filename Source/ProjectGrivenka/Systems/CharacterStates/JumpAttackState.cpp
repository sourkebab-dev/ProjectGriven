// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpAttackState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

bool UJumpAttackState::StateValidation_Implementation()
{
	return Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)
		&& Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp)
		&& !Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp)->IsMovingOnGround();
}


void UJumpAttackState::OnStateEnter_Implementation()
{
	auto CharacterActor = Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor);
	CharacterActor->LaunchCharacter(FVector(0.0f, 0.0f, 300.0f), false, true);
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->StatesComp->CompContext->CharacterActor, EAttackMovementType::AM_AIR, FAttackValues(), this->StatesComp->CompContext->CombatModule.CurrentAttack);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);
	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UJumpAttackState::OnAttackEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->StatesComp->CompContext->CombatModule.CurrentAttack.AttackMontage);

}

void UJumpAttackState::OnStateExit_Implementation()
{
}

void UJumpAttackState::OnAttackEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (this->CharMove->IsMovingOnGround()) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	}
	else {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.InAir"), EActionList::ActionNone, IE_Released);
	}
}
