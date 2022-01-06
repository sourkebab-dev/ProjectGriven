// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockedState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/VectorMathLib.h"

void UKnockedState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
	this->CharacterContext.EventBus->DamagedDelegate.AddDynamic(this, &UKnockedState::OnReceiveHit);
}

void UKnockedState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->CharacterContext.CharacterAnim->Montage_Stop(0.25);
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}
}

void UKnockedState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->PooledTime = 0.0;
	this->PushTargetLocation = FVector::ZeroVector;
	this->PushStartLocation = FVector::ZeroVector;
	GEngine->AddOnScreenDebugMessage(22, 1, FColor::Yellow, "hit");
	this->StatesComp->CrossStateData.IsInterruptable = false;
	if (!this->CharacterContext.CharacterAnim) { this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Pressed); return; }
	this->StartHitReact();
}

void UKnockedState::OnStateExit_Implementation()
{
	this->StatesComp->CrossStateData.IsInterruptable = true;
	if (!this->CharacterContext.CharacterAnim) return;
	FAnimMontageInstance* AnimMontageInstance = this->CharacterContext.CharacterAnim->GetActiveInstanceForMontage(this->CurrentStunMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->CharacterContext.CharacterAnim->Montage_Stop(0.25);
}

void UKnockedState::OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	this->HitInstigator = InHitInstigator;
	this->DamageInfo = InDamageInfo;
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"), EActionList::ActionNone, EInputEvent::IE_Released);
}

void UKnockedState::StartHitReact()
{
	float Degrees = UVectorMathLib::DegreesBetweenVectors(this->CharacterContext.CharacterActor->GetActorForwardVector(), this->HitInstigator->GetActorForwardVector());
	FVector StunDirection = this->DamageInfo.DamageDirection.RotateAngleAxis(Degrees, FVector::UpVector);
	StunDirection.Z *= -1;

	if (FVector::DotProduct(StunDirection, FVector::LeftVector) >= DOTDIRECTIONTRESHOLD) {
		this->CurrentStunMontage = this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH ? this->StunLeftMontage.HeavyMontage : this->StunLeftMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::RightVector) >= DOTDIRECTIONTRESHOLD) {
		this->CurrentStunMontage = this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH ? this->StunRightMontage.HeavyMontage : this->StunRightMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::UpVector) >= DOTDIRECTIONTRESHOLD) {
		this->CurrentStunMontage = this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH ? this->StunUpMontage.HeavyMontage : this->StunUpMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::DownVector) >= DOTDIRECTIONTRESHOLD) {
		this->CurrentStunMontage = this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH ? this->StunDownMontage.HeavyMontage : this->StunDownMontage.DefaultMontage;
	}
	else if (FVector::DotProduct(StunDirection, FVector::ForwardVector) >= DOTDIRECTIONTRESHOLD) {
		this->CurrentStunMontage = this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH ? this->StunFrontMontage.HeavyMontage : this->StunFrontMontage.DefaultMontage;
	}
	else {
		this->CurrentStunMontage = this->DamageInfo.ImpactType == EDamageImpactType::DI_HIGH ? this->StunBackMontage.HeavyMontage : this->StunBackMontage.DefaultMontage;
	}
	this->CharacterContext.CharacterAnim->Montage_Play(this->CurrentStunMontage);

	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UKnockedState::OnHitReactEnd);
	this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->CurrentStunMontage);
	this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->CurrentStunMontage);

	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->CharacterContext.MovementComp);
	if (!MovementComp) return;
	FVector KnockBackDir = this->HitInstigator->GetActorForwardVector();
	KnockBackDir.Normalize();
	KnockBackDir.Z = 0;
	this->PushStartLocation = this->CharacterContext.CharacterActor->GetActorLocation();
	this->PushTargetLocation = this->CharacterContext.CharacterActor->GetActorLocation() + (KnockBackDir * this->PushDistanceMultiplier);
}

void UKnockedState::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (this->PooledTime >= this->TotalPushTime) return;
	this->PooledTime += DeltaTime;
	FVector InterpVector = FMath::Lerp(this->PushStartLocation, this->PushTargetLocation, this->PooledTime / this->TotalPushTime);
	this->CharacterContext.CharacterActor->SetActorLocation(InterpVector, true);
}


void UKnockedState::OnHitReactEnd(UAnimMontage* Montage, bool bInterrupted)
{
	this->HitInstigator = nullptr;
	this->DamageInfo = FDamageInfo();
	if (!bInterrupted) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, EInputEvent::IE_Released);
	}
}
