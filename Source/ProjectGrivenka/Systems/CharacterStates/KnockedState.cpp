// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockedState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"
#include "Curves/CurveVector.h"

void UKnockedState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->StatesComp->CompContext->CharacterAnim->Montage_Stop(0.25);
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}
}

void UKnockedState::OnStateEnter_Implementation()
{
	Super::OnStateEnter_Implementation();
	this->StatesComp->CrossStateData.IsInterruptable = false;
	if (!this->StatesComp->CompContext->CharacterAnim) { this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Pressed); return; }
	this->StatesComp->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UKnockedState::OnProcessAnimRate);
	this->StartHitReact();
}

void UKnockedState::StartHitReact()
{
	this->CharMove = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	float DotValue = FVector::DotProduct(this->StatesComp->CompContext->CharacterActor->GetActorForwardVector(), this->StatesComp->CrossStateData.DamageInstigator->GetActorForwardVector());
	EHitDirectionType KnockbackDirection = DotValue < 0.3 && this->InversionMap.FindRef(this->StatesComp->CrossStateData.DamageInfo.DamageDirection)
		? this->InversionMap.FindRef(this->StatesComp->CrossStateData.DamageInfo.DamageDirection)
		: this->StatesComp->CrossStateData.DamageInfo.DamageDirection;

	FKnockBackDataContainer KnockbackSets = this->KnockbackData.FindRef(KnockbackDirection);
	int ImpactValue = this->ImpactModifier.FindRef(this->StatesComp->MinimumImpactForKnockback);
	int ModifiedValue = this->StatesComp->CrossStateData.DamageInfo.ImpactType + ImpactValue;
	EDamageImpactType ModifiedImpact = ModifiedValue > EDamageImpactType::DI_EXPLOSIVE ? EDamageImpactType::DI_EXPLOSIVE : static_cast<EDamageImpactType>(ModifiedValue);

	TArray<FKnockBackData> ChosenKnockBackSet;
	if (ModifiedImpact == EDamageImpactType::DI_MEDIUM) ChosenKnockBackSet = KnockbackSets.Standing;
	else if (ModifiedImpact == EDamageImpactType::DI_HIGH) ChosenKnockBackSet = KnockbackSets.FarPush;
	else  ChosenKnockBackSet = KnockbackSets.Launch;

	this->CurrentKnockbackData = ChosenKnockBackSet[FMath::RandRange(0, ChosenKnockBackSet.Num() - 1)];
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->CurrentKnockbackData.KnockBackMontage);

	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UKnockedState::OnHitReactEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->CurrentKnockbackData.KnockBackMontage);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->CurrentKnockbackData.KnockBackMontage);


	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, FTimerDelegate::CreateLambda([&] {
		if (this->StatesComp->CrossStateData.DamageInstigator) {
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->StatesComp->CrossStateData.DamageInstigator);
			InstigatorCtx->EventBus->HitStopDelegate.Execute(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([] {}));
		}

		//freeze -> onFreezefinish
		this->StatesComp->LockAnimation(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([&] {
			UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
			if (!MovementComp) return;
			if (this->CurrentKnockbackData.ApexType == EDirectionType::VERTICAL) {
				this->CharMove->SetMovementMode(EMovementMode::MOVE_Flying);
			}
			else if (this->CurrentKnockbackData.ApexType == EDirectionType::HORIZONTAL) {
				this->CharMove->BrakingDecelerationWalking = this->CurrentKnockbackData.NewBrakingDeceleration;
				this->CharMove->BrakingFrictionFactor = this->CurrentKnockbackData.NewFrictionFactor;
			}
			float min;
			this->CurrentKnockbackData.ForceCurve->GetTimeRange(min, this->ApexTime);
			if (this->CurrentKnockbackData.KnockBackMontage->GetSectionIndex("Launch") != INDEX_NONE) {
				float LaunchAnimDuration = this->CurrentKnockbackData.KnockBackMontage->GetSectionLength(this->CurrentKnockbackData.KnockBackMontage->GetSectionIndex("Launch"));
				float NewPlayRate = LaunchAnimDuration / this->ApexTime;
				this->CachedAnimRate = NewPlayRate;
			}

			if (this->StatesComp->CrossStateData.DamageInstigator) this->ForceDirection = this->StatesComp->CompContext->CharacterActor->GetActorLocation() - this->StatesComp->CrossStateData.DamageInstigator->GetActorLocation();
			else this->ForceDirection = this->StatesComp->CompContext->CharacterActor->GetActorForwardVector() * -1;
			this->ForceDirection.Normalize();


			this->IsLandingPartitioned = this->CurrentKnockbackData.KnockBackMontage->GetSectionIndex("Landing") != INDEX_NONE;
			this->IsFallingPartitioned = this->CurrentKnockbackData.KnockBackMontage->GetSectionIndex("Falling") != INDEX_NONE;
			this->IsProcessKnockback = true;
		}));

	}), 0.05, false);
}


void UKnockedState::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);
	if (this->IsProcessKnockback) {
		this->PooledTime += DeltaTime;
		if (this->PooledTime <= this->ApexTime) {
			FVector ForceValue = this->CurrentKnockbackData.ForceCurve->GetVectorValue(this->PooledTime);
			FVector FinalVelocity = FVector(1, 1, 1);
			FinalVelocity = this->ForceDirection * ForceValue.X;
			FinalVelocity.Z = ForceValue.Z;
			this->CharMove->Velocity = FinalVelocity;
		}
		else {
			this->CharMove->SetMovementMode(EMovementMode::MOVE_Falling);
			this->IsProcessKnockback = false;
			this->IsCurveEnd = true;


			if (this->IsFallingPartitioned) {
				//Note: Calculate playrate on falling
				FFindFloorResult Floor;
				this->CharMove->ComputeFloorDist(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), 10000, 10000, Floor, 10.0f);
				float ToFloorHeight = Floor.GetDistanceToFloor();
				float GravityZ = FMath::Abs(this->CharMove->GetGravityZ());
				float TimeToGround = (-this->CharMove->Velocity.Z + FMath::Sqrt(this->CharMove->Velocity.Z + 2 * ToFloorHeight * GravityZ)) / GravityZ;
				float LaunchAnimDuration = this->CurrentKnockbackData.KnockBackMontage->GetSectionLength(this->CurrentKnockbackData.KnockBackMontage->GetSectionIndex("Falling"));
				float NewPlayRate = LaunchAnimDuration / TimeToGround;
				GLog->Log("Falling");
				GLog->Log(FString::SanitizeFloat(NewPlayRate));
				this->CachedAnimRate = NewPlayRate;
			}
		}
	}

	if (this->IsCurveEnd && this->CharMove->MovementMode == EMovementMode::MOVE_Walking) {
		this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->CurrentKnockbackData.KnockBackMontage, 1.0f);
		if (this->IsLandingPartitioned) {
			this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("Landing", this->CurrentKnockbackData.KnockBackMontage);
		}
		this->IsCurveEnd = false;
	}
}


void UKnockedState::OnHitReactEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (this->IsInvalidated) return;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);

	this->IsInvalidated = true;

	if (!bInterrupted) {
		this->StatesComp->CrossStateData.DamageInstigator = nullptr;
		this->StatesComp->CrossStateData.DamageInfo = FDamageInfo();
		this->StatesComp->BlockedTags.RemoveAllTags();
		if (this->CurrentKnockbackData.KnockDownMontage) {
			this->StatesComp->CrossStateData.KnockDownMontage = this->CurrentKnockbackData.KnockDownMontage;
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.KnockedDown"), EActionList::ActionNone, IE_Pressed);
		}
		else {
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, EInputEvent::IE_Released);
		}
	}

	
}

void UKnockedState::OnProcessAnimRate(EAnimEvt EventType)
{
	if (EventType != EAnimEvt::PROCESS_ANIMRATE) return;
	this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->CurrentKnockbackData.KnockBackMontage, this->CachedAnimRate);
}

void UKnockedState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->EventBus->AnimDelegate.RemoveDynamic(this, &UKnockedState::OnProcessAnimRate);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CompContext->CharacterAnim->Montage_Stop(0.25);
	this->CharMove->BrakingDecelerationWalking = DEFAULTGROUNDFRICTION;
	this->CharMove->BrakingFrictionFactor = DEFAULTBRAKINGDECELERATION;
	this->StatesComp->CompContext->CharacterAnim->Montage_SetPlayRate(this->CurrentKnockbackData.KnockBackMontage, 1.0f);
}
