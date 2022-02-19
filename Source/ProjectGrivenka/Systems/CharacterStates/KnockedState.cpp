// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockedState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"

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
	this->PooledTime = 0.0;
	this->PushTargetLocation = FVector::ZeroVector;
	this->PushStartLocation = FVector::ZeroVector;
	this->StatesComp->CrossStateData.IsInterruptable = false;
	if (!this->StatesComp->CompContext->CharacterAnim) { this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Pressed); return; }
	this->StartHitReact();
}

void UKnockedState::StartHitReact()
{
	float Degrees = UVectorMathLib::DegreesBetweenVectors(this->StatesComp->CompContext->CharacterActor->GetActorForwardVector(), this->StatesComp->CrossStateData.DamageInstigator->GetActorForwardVector());
	FVector StunDirection = this->StatesComp->CrossStateData.DamageInfo.DamageDirection.RotateAngleAxis(Degrees, FVector::UpVector);
	StunDirection.Z *= -1;

	if (FVector::DotProduct(StunDirection, FVector::LeftVector) >= DOTDIRECTIONTRESHOLD) {
		GEngine->AddOnScreenDebugMessage(41, 2, FColor::Yellow, "Left");
		this->CurrentStunMontage = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= this->StatesComp->HeavyKnockedByWhichImpact ? this->StunLeftMontage.HeavyMontage : this->StunLeftMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::RightVector) >= DOTDIRECTIONTRESHOLD) {
		GEngine->AddOnScreenDebugMessage(41, 2, FColor::Yellow, "Right");
		this->CurrentStunMontage = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= this->StatesComp->HeavyKnockedByWhichImpact ? this->StunRightMontage.HeavyMontage : this->StunRightMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::UpVector) >= DOTDIRECTIONTRESHOLD) {
		GEngine->AddOnScreenDebugMessage(41, 2, FColor::Yellow, "Up");
		this->CurrentStunMontage = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= this->StatesComp->HeavyKnockedByWhichImpact ? this->StunUpMontage.HeavyMontage : this->StunUpMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::DownVector) >= DOTDIRECTIONTRESHOLD) {
		GEngine->AddOnScreenDebugMessage(41, 2, FColor::Yellow, "Down");
		this->CurrentStunMontage = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= this->StatesComp->HeavyKnockedByWhichImpact ? this->StunDownMontage.HeavyMontage : this->StunDownMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::ForwardVector) >= DOTDIRECTIONTRESHOLD) {
		GEngine->AddOnScreenDebugMessage(41, 2, FColor::Yellow, "Front");
		this->CurrentStunMontage = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= this->StatesComp->HeavyKnockedByWhichImpact ? this->StunFrontMontage.HeavyMontage : this->StunFrontMontage.DefaultMontage;
	}
	else {
		GEngine->AddOnScreenDebugMessage(41, 2, FColor::Yellow, "Back");
		this->CurrentStunMontage = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= this->StatesComp->HeavyKnockedByWhichImpact ? this->StunBackMontage.HeavyMontage : this->StunBackMontage.DefaultMontage;
	}
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->CurrentStunMontage);

	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UKnockedState::OnHitReactEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->CurrentStunMontage);
	//this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->CurrentStunMontage);


	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, FTimerDelegate::CreateLambda([&] {
		if (this->StatesComp->CrossStateData.DamageInstigator) {
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->StatesComp->CrossStateData.DamageInstigator);
			InstigatorCtx->EventBus->HitStopDelegate.Execute(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([] {}));
		}

		//freeze -> onFreezefinish
		this->StatesComp->LockAnimation(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([&] {
			UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
			if (!MovementComp) return;
			FVector KnockBackDir = this->StatesComp->CrossStateData.DamageInstigator->GetActorForwardVector();
			KnockBackDir.Normalize();
			KnockBackDir.Z = 0;
			this->PushStartLocation = this->StatesComp->CompContext->CharacterActor->GetActorLocation();
			this->PushTargetLocation = this->StatesComp->CompContext->CharacterActor->GetActorLocation() + (KnockBackDir * this->PushDistanceMultiplier);
		}));

	}), 0.005, false);
}

void UKnockedState::OnHitReactEnd(UAnimMontage* Montage, bool bInterrupted)
{
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);

	if (!bInterrupted) {
		this->StatesComp->CrossStateData.DamageInstigator = nullptr;
		this->StatesComp->CrossStateData.DamageInfo = FDamageInfo();
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, EInputEvent::IE_Released);
	}
}

void UKnockedState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	this->StatesComp->CrossStateData.IsInterruptable = true;
	if (!this->StatesComp->CompContext->CharacterAnim) return;
	FAnimMontageInstance* AnimMontageInstance = this->StatesComp->CompContext->CharacterAnim->GetActiveInstanceForMontage(this->CurrentStunMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->StatesComp->CompContext->CharacterAnim->Montage_Stop(0.25);
}

void UKnockedState::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);
	if (this->PushTargetLocation.IsZero() || this->PooledTime >= this->TotalPushTime) return;
	this->PooledTime += DeltaTime;
	FVector InterpVector = FMath::Lerp(this->PushStartLocation, this->PushTargetLocation, this->PooledTime / this->TotalPushTime);
	this->StatesComp->CompContext->CharacterActor->SetActorLocation(InterpVector, true);
}


