// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockPushState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"


bool UBlockPushState::StateValidation_Implementation()
{
	return this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>();
}


void UBlockPushState::OnStateEnter_Implementation()
{
	FBlockInfo BlockInfo;
	float DamageAbsorption;
	IEquipmentSystemAvailable::Execute_GetBlockInfo(this->StatesComp->CompContext->CharacterActor, BlockInfo, DamageAbsorption);
	if (!BlockInfo.BlockPushMontage) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}
	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UBlockPushState::OnPushEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, BlockInfo.BlockPushMontage);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(BlockInfo.BlockPushMontage);

	this->StatesComp->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UBlockPushState::OnBlockPushTriggered);
}

void UBlockPushState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->EventBus->AnimDelegate.RemoveDynamic(this, &UBlockPushState::OnBlockPushTriggered);
}

void UBlockPushState::OnBlockPushTriggered(EAnimEvt EventType)
{
	if (EventType != EAnimEvt::BLOCK_PUSH) return;


	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutResults;
	ActorsToIgnore.Add(this->StatesComp->CompContext->CharacterActor);

	if (UKismetSystemLibrary::SphereTraceMulti(this->GetWorld(), this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->StatesComp->CompContext->CharacterActor->GetActorLocation(), 200.0f,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::None, OutResults, true)) {
		for (int i = 0; i < OutResults.Num(); i++) {
			AActor* HitActor = OutResults[i].Actor.Get();
			if (!HitActor 
				|| !IAIContextSystemAvailable::Execute_CheckHostility(this->StatesComp->CompContext->CharacterActor, HitActor) 
				|| !UVectorMathLib::CheckBlockDirection(HitActor->GetActorLocation(), this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->StatesComp->CompContext->CharacterActor->GetActorForwardVector())) continue;

			auto InstigatorCtx = IContextAvailable::Execute_GetContext(HitActor);
			if (!InstigatorCtx->EventBus) return;


			FDamageInfo StaggerDamage;
			StaggerDamage.ImpactType = EDamageImpactType::DI_HIGH;
			StaggerDamage.HitType = EDamageHitType::PUSH;
			InstigatorCtx->EventBus->HitDelegate.Broadcast(this->StatesComp->CompContext->CharacterActor, StaggerDamage);
		}
	}


}

void UBlockPushState::OnPushEnd(UAnimMontage* Montage, bool bInterrupted)
{
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
}
