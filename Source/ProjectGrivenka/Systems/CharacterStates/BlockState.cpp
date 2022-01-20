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
	this->StatesComp->BlockHitDelegate.AddDynamic(this, &UBlockState::OnReceiveHit);

	FBlockInfo BlockInfo;
	float DamageAbsorption;
	IEquipmentSystemAvailable::Execute_GetBlockInfo(this->CharacterContext.CharacterActor, BlockInfo, DamageAbsorption);
	this->CharacterContext.CharacterAnim->Montage_Play(BlockInfo.BlockMontage);
	this->IsParry = true;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->ParryTimer, this, &UBlockState::InvalidateParry, PARRYTIME);
	
	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectByPrefabName(this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor, "Util_Parry", 0, false);
	}

	if (this->CharacterContext.MovementComp) {
		UCharacterMovementComponent* CharMove = Cast<UCharacterMovementComponent>(this->CharacterContext.MovementComp);
		this->TempMaxWalkSpeed = CharMove->MaxWalkSpeed;
		CharMove->MaxWalkSpeed = CharMove->MaxWalkSpeed * 0.3;
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
	if (this->CharacterContext.MovementComp) {
		UCharacterMovementComponent* CharMove = Cast<UCharacterMovementComponent>(this->CharacterContext.MovementComp);
		CharMove->MaxWalkSpeed = this->TempMaxWalkSpeed;
	}
	this->StatesComp->BlockHitDelegate.RemoveDynamic(this, &UBlockState::OnReceiveHit);
	this->CharacterContext.CharacterAnim->StopAllMontages(0.1);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->ParryTimer);
	ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->CharacterContext.CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Block"));
	ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->CharacterContext.CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Parry"));
}

void UBlockState::OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	if (this->StatesComp->BlockedTags.HasAny(this->ActionTag)) return;

	this->HitInstigator = InHitInstigator;
	this->DamageInfo = InDamageInfo;

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
		if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
			ICharacterSystemAvailable::Execute_InitEffectFortitudeDamage(this->CharacterContext.CharacterActor, InHitInstigator, this->DamageInfo);
			float CurrentFortitude = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Fortitude);
			float MaxFortitude = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Fortitude);


			if (CurrentFortitude <= 0) {
				this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Staggered"), EActionList::ActionNone, EInputEvent::IE_Released);
			}
		}


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