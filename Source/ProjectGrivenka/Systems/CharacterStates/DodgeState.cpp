// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"


void UDodgeState::Init_Implementation(UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InStatesComp);
	FOnTimelineFloat DodgeTick_ImplementationDelegate;
	FOnTimelineEvent FinishDelegate;
	DodgeTick_ImplementationDelegate.BindUFunction(this, "OnDodging");
	FinishDelegate.BindUFunction(this, "OnFinishDodging");
	if (!IsValid(this->DodgeCurve)) { UE_LOG(LogTemp, Error, TEXT("Invalid Curve Float"), *GetNameSafe(this)); return; }
	this->DodgeTimeline = FTimeline{};
	this->DodgeTimeline.SetLooping(false);
	this->DodgeTimeline.AddInterpFloat(this->DodgeCurve, DodgeTick_ImplementationDelegate);
	this->DodgeTimeline.SetTimelineFinishedFunc(FinishDelegate);
}

bool UDodgeState::StateValidation_Implementation() {
	return this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->StatesComp->CompContext->CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void UDodgeState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionAttack && EventType == IE_Pressed) {
		this->StatesComp->CrossStateData.IsLungePooling = true;
	}

}

//SPONGE: Need to add setter for calculating walkspeed
void UDodgeState::OnStateEnter_Implementation()
{
	Super::OnStateEnter_Implementation();
	this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir = this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir.IsZero()
		? this->StatesComp->CompContext->CharacterActor->GetActorForwardVector() * -1 : this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir;
	this->TempCurrentLocation = this->StatesComp->CompContext->CharacterActor->GetActorLocation();
	this->DodgeTargetLocation = this->TempCurrentLocation + this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir * 250.0;
	this->DodgeTimeline.PlayFromStart();
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::START_DODGE);

	//SPONGE: i should probably adjust this setting but it works anyway lol
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	this->TempWalkSpeed = MovementComp->MaxWalkSpeed;
	this->TempGroundFriction = MovementComp->BrakingDecelerationWalking;
	this->TempAcceleration = MovementComp->MaxAcceleration;
	this->TempBrakingFriction = MovementComp->BrakingFrictionFactor;
	MovementComp->BrakingFrictionFactor = 10;
	MovementComp->BrakingDecelerationWalking = 3000;
	MovementComp->MaxAcceleration = 3000;
	MovementComp->MaxWalkSpeed = 999999;

	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectDepleteStamina(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CharacterActor, 10);
	}


	this->StatesComp->CrossStateData.IsLungeAvailable = false;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->LungeTimer);

	this->SetDamageCollider(false);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->DodgeTimer, FTimerDelegate::CreateLambda([&] {
		this->SetDamageCollider(true);
	}), 0.25, false);
}

void UDodgeState::Tick_Implementation(float DeltaTime) {
	if (this->DodgeTimeline.IsPlaying()) {
		this->DodgeTimeline.TickTimeline(DeltaTime);
	}
}

void UDodgeState::OnStateExit_Implementation()
{
	this->TempCurrentLocation = FVector::ZeroVector;
	this->DodgeTargetLocation = FVector::ZeroVector;
	this->DodgeTimeline.Stop();

	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->StatesComp->CompContext->MovementComp);
	MovementComp->MaxWalkSpeed = this->TempWalkSpeed;
	MovementComp->MaxAcceleration = this->TempAcceleration;
	MovementComp->BrakingDecelerationWalking = this->TempGroundFriction;
	MovementComp->BrakingFrictionFactor = this->TempBrakingFriction;
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::END_DODGE);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->DodgeTimer);
	this->SetDamageCollider(true);
}

void UDodgeState::OnDodging(float InterpValue) {
	FVector Interpolation = FMath::Lerp(this->TempCurrentLocation, this->DodgeTargetLocation, InterpValue);
	this->StatesComp->CompContext->MovementComp->AddInputVector(Interpolation - this->StatesComp->CompContext->CharacterActor->GetActorLocation());
	GLog->Log("OnDodging");
}

void UDodgeState::OnFinishDodging() {
	this->StatesComp->CrossStateData.IsLungeAvailable = true;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->LungeTimer, this, &UDodgeState::DeactivateLunge, 0.2, false, -1.0f);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::END_DODGE);
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	GLog->Log("FinishDodge");
}

void UDodgeState::DeactivateLunge()
{
	this->StatesComp->CrossStateData.IsLungePooling = false;
	this->StatesComp->CrossStateData.IsLungeAvailable = false;
}

void UDodgeState::SetDamageCollider(bool IsActive)
{
	//sponge: might be bad perf
	TArray<USceneComponent*> DamageColliders;
	this->StatesComp->CompContext->SkeletalMeshComp->GetChildrenComponents(false, DamageColliders);
	for (int i = 0; i < DamageColliders.Num(); i++) {
		auto Collider = Cast<UCapsuleComponent>(DamageColliders[i]);
		if (!Collider) continue;
		Collider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, IsActive ? ECollisionResponse::ECR_Overlap : ECollisionResponse::ECR_Ignore);
	}
}
