// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/VectorMathLib.h"


//sponge: Might need to wait for block hit to end if release block in the middle of getting hit
void UBlockState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) 
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->CharacterContext.EventBus->DamagedDelegate.AddDynamic(this, &UBlockState::OnReceiveHit);

	FBlockMontages BlockMontages;
	IEquipmentSystemAvailable::Execute_GetBlockMontages(this->CharacterContext.CharacterActor, BlockMontages);
	this->CharacterContext.CharacterAnim->Montage_Play(BlockMontages.BlockMontage);
	this->IsParry = true;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->ParryTimer, this, &UBlockState::InvalidateParry, PARRYTIME);
}

void UBlockState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionBlock && EventType == IE_Released) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), Action, EventType);
	}
}

void UBlockState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
	if (Action == EActionList::ActionMoveForward || Action == EActionList::ActionMoveRight)
	{
		if (!this->CharacterContext.MovementComp) return;
		this->CharacterContext.MovementComp->AddInputVector(this->CharacterContext.Store->MovementModule.WorldSpaceTargetDir);
	}
}

void UBlockState::OnStateExit_Implementation()
{
	this->CharacterContext.EventBus->DamagedDelegate.RemoveDynamic(this, &UBlockState::OnReceiveHit);
	this->CharacterContext.CharacterAnim->StopAllMontages(0.1);
	this->ParryTimer.Invalidate();
}

void UBlockState::OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	this->HitInstigator = InHitInstigator;
	this->DamageInfo = InDamageInfo;

	if (UVectorMathLib::CheckBlockDirection(InHitInstigator->GetActorLocation(), this->CharacterContext.CharacterActor->GetActorLocation(), this->CharacterContext.CharacterActor->GetActorForwardVector())) {
		FBlockMontages BlockMontages;
		IEquipmentSystemAvailable::Execute_GetBlockMontages(this->CharacterContext.CharacterActor, BlockMontages);

		if (this->IsParry) {
			this->CharacterContext.CharacterAnim->Montage_JumpToSection("Parry", BlockMontages.BlockMontage);
		}
		else {
			this->CharacterContext.CharacterAnim->Montage_JumpToSection("Hit", BlockMontages.BlockMontage);
		}
	}
	else {
		this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Ground"));
		this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"));
		this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Air"));
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"), EActionList::ActionNone, EInputEvent::IE_Released);
	}
}

void UBlockState::InvalidateParry()
{
	this->IsParry = false;
	//sponge: disable ucharsys parry eff
}

bool UBlockState::StateValidation_Implementation()
{
	return this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>();
}
