// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockedState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"

void UKnockedState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
	this->StatesComp->TrueHitDelegate.AddDynamic(this, &UKnockedState::OnReceiveHit);
}

void UKnockedState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->CharacterContext.CharacterAnim->Montage_Stop(0.25);
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}
}

void UKnockedState::OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	if (this->StatesComp->BlockedTags.HasAny(this->ActionTag)) return;

	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	if (this->HitInstigator) {
		this->ClearPauseOnLastInstigator();
	}
	this->HitInstigator = InHitInstigator;
	this->DamageInfo = InDamageInfo;

	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectFortitudeDamage(this->CharacterContext.CharacterActor, InHitInstigator, this->DamageInfo);
		float CurrentFortitude = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Fortitude);
		float MaxFortitude = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Fortitude);


		if (CurrentFortitude <= 0 && this->IsStaggeredOnEmptyFortitude) {
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Staggered"), EActionList::ActionNone, EInputEvent::IE_Released);
		}
		else if (CurrentFortitude / MaxFortitude < 0.6) {
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"), EActionList::ActionNone, EInputEvent::IE_Released);
		}
		else {
			//Sponge: need to add jitter effect
		}
	}
	else {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"), EActionList::ActionNone, EInputEvent::IE_Released);
	}

}

void UKnockedState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->PooledTime = 0.0;
	this->PushTargetLocation = FVector::ZeroVector;
	this->PushStartLocation = FVector::ZeroVector;
	this->StatesComp->CrossStateData.IsInterruptable = false;
	if (!this->CharacterContext.CharacterAnim) { this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Pressed); return; }
	this->StartHitReact();
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
	//this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->CurrentStunMontage);


	this->StatesComp->BlockedTags.AddTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"));
	//Sponge: try hitpause
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, this, &UKnockedState::InitiatePause, 0.005);
}

void UKnockedState::InitiatePause()
{
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	this->CharacterContext.CharacterAnim->Montage_Pause(this->CurrentStunMontage);

	float PauseTime = 0.0;
	switch (this->DamageInfo.ImpactType)
	{
	case EDamageImpactType::DI_HIGH:
		PauseTime = 0.2;
		break;
	case EDamageImpactType::DI_MEDIUM:
		PauseTime = 0.1;
		break;
	case EDamageImpactType::DI_LOW:
		PauseTime = 0.07;
		break;
	default:
		break;
	}
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, this, &UKnockedState::OnPauseEnd, PauseTime);

	if (!this->HitInstigator) {
		GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "Knock No Instigator");
		GLog->Log("Knock No Instigator");
		return;
	}
	FCharacterContext InstigatorCtx;
	IContextAvailable::Execute_GetContext(this->HitInstigator, InstigatorCtx);
	InstigatorCtx.CharacterAnim->Montage_Pause(InstigatorCtx.CharacterAnim->GetCurrentActiveMontage());
}

void UKnockedState::OnPauseEnd()
{
	this->StatesComp->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"));
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	this->ClearPauseOnLastInstigator();

	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->CharacterContext.MovementComp);
	if (!MovementComp) return;
	FVector KnockBackDir = this->HitInstigator->GetActorForwardVector();
	KnockBackDir.Normalize();
	KnockBackDir.Z = 0;
	this->PushStartLocation = this->CharacterContext.CharacterActor->GetActorLocation();
	this->PushTargetLocation = this->CharacterContext.CharacterActor->GetActorLocation() + (KnockBackDir * this->PushDistanceMultiplier);

}

void UKnockedState::OnHitReactEnd(UAnimMontage* Montage, bool bInterrupted)
{
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);

	if (!bInterrupted) {
		this->HitInstigator = nullptr;
		this->DamageInfo = FDamageInfo();
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, EInputEvent::IE_Released);
	}
	else {
		this->ClearPauseOnLastInstigator();
	}

}

void UKnockedState::ClearPauseOnLastInstigator()
{
	this->CharacterContext.CharacterAnim->Montage_Resume(this->CurrentStunMontage);
	if (!this->HitInstigator) {
		GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "Knock No Instigator");
		GLog->Log("Knock No Instigator");
		return;
	}
	FCharacterContext InstigatorCtx;
	IContextAvailable::Execute_GetContext(this->HitInstigator, InstigatorCtx);
	InstigatorCtx.CharacterAnim->Montage_Resume(InstigatorCtx.CharacterAnim->GetCurrentActiveMontage());

}

void UKnockedState::OnStateExit_Implementation()
{
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	this->StatesComp->CrossStateData.IsInterruptable = true;
	if (!this->CharacterContext.CharacterAnim) return;
	FAnimMontageInstance* AnimMontageInstance = this->CharacterContext.CharacterAnim->GetActiveInstanceForMontage(this->CurrentStunMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->CharacterContext.CharacterAnim->Montage_Stop(0.25);
}

void UKnockedState::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);
	if (this->PushTargetLocation.IsZero() || this->PooledTime >= this->TotalPushTime) return;
	this->PooledTime += DeltaTime;
	FVector InterpVector = FMath::Lerp(this->PushStartLocation, this->PushTargetLocation, this->PooledTime / this->TotalPushTime);
	this->CharacterContext.CharacterActor->SetActorLocation(InterpVector, true);
}


