// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/VectorMathLib.h"


//sponge: Might need to wait for block hit to end if release block in the middle of getting hit
void UBlockState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) 
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->CharacterContext.EventBus->DamagedDelegate.AddDynamic(this, &UBlockState::OnReceiveHit);

	FBlockInfo BlockInfo;
	float DamageAbsorption;
	IEquipmentSystemAvailable::Execute_GetBlockInfo(this->CharacterContext.CharacterActor, BlockInfo, DamageAbsorption);
	this->CharacterContext.CharacterAnim->Montage_Play(BlockInfo.BlockMontage);
	this->IsParry = true;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->ParryTimer, this, &UBlockState::InvalidateParry, PARRYTIME);
	
	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectByPrefabName(this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor, "Util_Parry", 0, false);
	}
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
	ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->CharacterContext.CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Block"));
	ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->CharacterContext.CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Parry"));
}

void UBlockState::OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	this->HitInstigator = InHitInstigator;
	this->DamageInfo = InDamageInfo;

	if (!UVectorMathLib::CheckBlockDirection(InHitInstigator->GetActorLocation(), this->CharacterContext.CharacterActor->GetActorLocation(), this->CharacterContext.CharacterActor->GetActorForwardVector())) {
		this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Ground"));
		this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"));
		this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Air"));
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"), EActionList::ActionNone, EInputEvent::IE_Released);
		return;
	}


	FBlockInfo BlockInfo;
	float DamageAbsorption;
	IEquipmentSystemAvailable::Execute_GetBlockInfo(this->CharacterContext.CharacterActor, BlockInfo, DamageAbsorption);

	if (this->IsParry) {
		this->CharacterContext.CharacterAnim->Montage_JumpToSection("Parry", BlockInfo.BlockMontage);

		FCharacterContext InstigatorCtx;
		IContextAvailable::Execute_GetContext(this->HitInstigator, InstigatorCtx);
		if (!InstigatorCtx.EventBus) return;

		FDamageInfo StaggerDamage;
		StaggerDamage.ImpactType = EDamageImpactType::DI_HIGH;
		if (this->HitInstigator->Implements<UCharacterSystemAvailable>()) {
			ICharacterSystemAvailable::Execute_InitEffectFortitudeDamage(InHitInstigator, this->CharacterContext.CharacterActor, StaggerDamage);
			float InstigatorFortitude = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(InHitInstigator, EAttributeCode::ATT_Fortitude);
			if (InstigatorFortitude <= 0) {
				InstigatorCtx.EventBus->StaggerDelegate.Broadcast(this->CharacterContext.CharacterActor, StaggerDamage);
			}
		}
		else {
			InstigatorCtx.EventBus->StaggerDelegate.Broadcast(this->CharacterContext.CharacterActor, StaggerDamage);

		}
	}
	else {
		this->CharacterContext.CharacterAnim->Montage_JumpToSection("Hit", BlockInfo.BlockMontage);
	}
}

void UBlockState::InvalidateParry()
{
	this->IsParry = false;
	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->CharacterContext.CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Parry"));
		FBlockInfo BlockInfo;
		float DamageAbsorption;
		IEquipmentSystemAvailable::Execute_GetBlockInfo(this->CharacterContext.CharacterActor, BlockInfo, DamageAbsorption);
		ICharacterSystemAvailable::Execute_InitEffectByPrefabName(this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor, "Util_RaiseShield", DamageAbsorption, true);
	}
}

bool UBlockState::StateValidation_Implementation()
{
	return this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>();
}
