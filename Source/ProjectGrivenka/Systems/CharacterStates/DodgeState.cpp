// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void UDodgeState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
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
	return this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void UDodgeState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	if (Action == EActionList::ActionAttack && EventType == IE_Pressed) {
		this->StatesComp->CrossStateData.IsLungePooling = true;
	}

}

//SPONGE: Need to add setter for calculating walkspeed
void UDodgeState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->CharacterContext.Store->MovementModule.WorldSpaceTargetDir = this->CharacterContext.Store->MovementModule.WorldSpaceTargetDir.IsZero()
		? this->CharacterContext.CharacterActor->GetActorForwardVector() * -1 : this->CharacterContext.Store->MovementModule.WorldSpaceTargetDir;
	this->TempCurrentLocation = this->CharacterContext.CharacterActor->GetActorLocation();
	this->DodgeTargetLocation = this->TempCurrentLocation + this->CharacterContext.Store->MovementModule.WorldSpaceTargetDir * 250.0;
	this->DodgeTimeline.PlayFromStart();
	this->CharacterContext.EventBus->AnimDelegate.Broadcast(EAnimEvt::START_DODGE);

	//SPONGE: i should probably adjust this setting but it works anyway lol
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->CharacterContext.MovementComp);
	this->TempWalkSpeed = MovementComp->MaxWalkSpeed;
	this->TempGroundFriction = MovementComp->BrakingDecelerationWalking;
	this->TempAcceleration = MovementComp->MaxAcceleration;
	this->TempBrakingFriction = MovementComp->BrakingFrictionFactor;
	MovementComp->BrakingFrictionFactor = 10;
	MovementComp->BrakingDecelerationWalking = 3000;
	MovementComp->MaxAcceleration = 3000;
	MovementComp->MaxWalkSpeed = 999999;

	if (this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectDepleteStamina(this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor, 10);
	}


	this->StatesComp->CrossStateData.IsLungeAvailable = false;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->LungeTimer);
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

	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->CharacterContext.MovementComp);
	MovementComp->MaxWalkSpeed = this->TempWalkSpeed;
	MovementComp->MaxAcceleration = this->TempAcceleration;
	MovementComp->BrakingDecelerationWalking = this->TempGroundFriction;
	MovementComp->BrakingFrictionFactor = this->TempBrakingFriction;
	this->CharacterContext.EventBus->AnimDelegate.Broadcast(EAnimEvt::END_DODGE);
}

void UDodgeState::OnDodging(float InterpValue) {
	FVector Interpolation = FMath::Lerp(this->TempCurrentLocation, this->DodgeTargetLocation, InterpValue);
	this->CharacterContext.MovementComp->AddInputVector(Interpolation - this->CharacterContext.CharacterActor->GetActorLocation());
	GLog->Log("OnDodging");
}

void UDodgeState::OnFinishDodging() {
	this->StatesComp->CrossStateData.IsLungeAvailable = true;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->LungeTimer, this, &UDodgeState::DeactivateLunge, 0.2, false, -1.0f);
	this->CharacterContext.EventBus->AnimDelegate.Broadcast(EAnimEvt::END_DODGE);
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	GLog->Log("FinishDodge");
}

void UDodgeState::DeactivateLunge()
{
	this->StatesComp->CrossStateData.IsLungePooling = false;
	this->StatesComp->CrossStateData.IsLungeAvailable = false;
}