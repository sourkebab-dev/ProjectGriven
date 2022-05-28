// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockState.h"
#include "MorphToolsFunctions.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/VectorMathLib.h"


//sponge: Might need to wait for block hit to end if release block in the middle of getting hit
void UBlockState::OnStateEnter_Implementation() 
{
	Super::OnStateEnter_Implementation();
	this->StatesComp->BlockHitDelegate.AddDynamic(this, &UBlockState::OnReceiveHit);


	FBlockInfo BlockInfo;
	float DamageAbsorption;
	IEquipmentSystemAvailable::Execute_GetBlockInfo(this->StatesComp->CompContext->CharacterActor, BlockInfo, DamageAbsorption);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(BlockInfo.BlockMontage);
	this->StatesComp->CrossStateData.IsParry = true;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->ParryTimer, this, &UBlockState::InvalidateParry, PARRYTIME);
	
	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectByPrefabName(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CharacterActor, "Util_Parry", 0, false);
	}

	if (this->StatesComp->CompContext->MovementComp) {
		UCharacterMovementComponent* CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
		this->TempMaxWalkSpeed = CharMove->MaxWalkSpeed;
		CharMove->MaxWalkSpeed = CharMove->MaxWalkSpeed * 0.3;
	}
}

void UBlockState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionBlock && EventType == IE_Released) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), Action, EventType);
	}

	if (Action == EActionList::ActionAttack && EventType == IE_Pressed) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.BlockPush"), Action, EventType);
	}
}

void UBlockState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
	if (Action == EActionList::ActionMoveForward || Action == EActionList::ActionMoveRight)
	{
		if (!this->StatesComp->CompContext->MovementComp) return;
		this->StatesComp->CompContext->MovementComp->AddInputVector(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir);
	}
}

void UBlockState::OnStateExit_Implementation()
{
	if (this->StatesComp->CompContext->MovementComp) {
		UCharacterMovementComponent* CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
		CharMove->MaxWalkSpeed = this->TempMaxWalkSpeed;
		CharMove->BrakingFrictionFactor = DEFAULTGROUNDFRICTION;
		CharMove->BrakingDecelerationWalking = DEFAULTBRAKINGDECELERATION;
	}
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->SkidTimer);
	this->StatesComp->BlockHitDelegate.RemoveDynamic(this, &UBlockState::OnReceiveHit);
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.1);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->ParryTimer);
	ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->StatesComp->CompContext->CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Block"));
	ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->StatesComp->CompContext->CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Parry"));
}

void UBlockState::OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	if (this->StatesComp->BlockedTags.HasAny(this->ActionTag)) return;

	this->HitInstigator = InHitInstigator;
	this->DamageInfo = InDamageInfo;

	FBlockInfo BlockInfo;
	float DamageAbsorption;
	IEquipmentSystemAvailable::Execute_GetBlockInfo(this->StatesComp->CompContext->CharacterActor, BlockInfo, DamageAbsorption);


	if (this->StatesComp->CrossStateData.IsParry) {
		FName ParrySectionName = this->ParryMap.FindRef(this->DamageInfo.DamageDirection);
		if (ParrySectionName.IsNone()) ParrySectionName = FMath::RandBool() ? "ParryLeft" : "ParryRight";
		this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection(ParrySectionName, BlockInfo.BlockMontage);

		auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->HitInstigator);
		if (!InstigatorCtx->EventBus) return;

		FDamageInfo StaggerDamage;
		StaggerDamage.ImpactType = EDamageImpactType::DI_HIGH;
		StaggerDamage.HitType = EDamageHitType::PARRY;
		InstigatorCtx->EventBus->HitDelegate.Broadcast(this->StatesComp->CompContext->CharacterActor, StaggerDamage);
	}
	else {
		this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("Hit", BlockInfo.BlockMontage);
	}

	auto CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	auto CharActor = Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor);
	CharMove->BrakingFrictionFactor = 0;
	CharMove->BrakingDecelerationWalking = this->Deceleration;
	FVector ForceDir = this->StatesComp->CompContext->CharacterActor->GetActorLocation() - this->HitInstigator->GetActorLocation();
	ForceDir.Normalize();
	float PushForce;


	if (this->StatesComp->CrossStateData.IsParry || this->DamageInfo.ImpactType == EDamageImpactType::DI_LOW) PushForce = this->LowImpactLaunchForce;
	else if (this->DamageInfo.ImpactType == EDamageImpactType::DI_MEDIUM) PushForce = this->MediumImpactLaunchForce;
	else if (this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH) PushForce = this->HighImpactLaunchForce;
	else PushForce = this->ExplosiveImpactLaunchForce;

	float SkidTime = PushForce / this->Deceleration;
	CharMove->AddImpulse(ForceDir * PushForce, true);

	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->SkidTimer, FTimerDelegate::CreateLambda([CharMove] {
		CharMove->BrakingFrictionFactor = DEFAULTGROUNDFRICTION;
		CharMove->BrakingDecelerationWalking = DEFAULTBRAKINGDECELERATION;
	}), SkidTime, false);

}

void UBlockState::InvalidateParry()
{
	this->StatesComp->CrossStateData.IsParry = false;
	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->StatesComp->CompContext->CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Parry"));
		FBlockInfo BlockInfo;
		float DamageAbsorption;
		IEquipmentSystemAvailable::Execute_GetBlockInfo(this->StatesComp->CompContext->CharacterActor, BlockInfo, DamageAbsorption);
		ICharacterSystemAvailable::Execute_InitEffectByPrefabName(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CharacterActor, "Util_RaiseShield", DamageAbsorption, true);
	}
}

bool UBlockState::StateValidation_Implementation()
{
	return this->StatesComp->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>();
}
