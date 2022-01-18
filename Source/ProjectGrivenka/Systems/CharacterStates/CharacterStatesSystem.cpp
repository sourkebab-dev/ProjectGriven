// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatesSystem.h"
#include "BaseState.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"

UCharacterStatesSystem::UCharacterStatesSystem() : UBaseContextableComponent() {
	this->PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterStatesSystem::Init()
{
	Super::Init();
	this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	this->InitializePersistantStates();
	if (!this->CompContext.EventBus) return;
	this->CompContext.EventBus->StateActionDelegate.AddDynamic(this, &UCharacterStatesSystem::CurrentActionHandler);
	this->CompContext.EventBus->StateAxisDelegate.AddDynamic(this, &UCharacterStatesSystem::CurrentAxisHandler);
	this->CompContext.EventBus->AnimDelegate.AddDynamic(this, &UCharacterStatesSystem::AnimEventsHandler);
	this->CompContext.EventBus->DamagedDelegate.AddDynamic(this, &UCharacterStatesSystem::OnHit);
}

void UCharacterStatesSystem::InitializePersistantStates()
{
	//sponge: might remove function when parallel/multiple states is implemented
	for (TSubclassOf<UBaseState> StateClass : this->GrantedActions) {
		UBaseState* DefaultObj = Cast<UBaseState>(StateClass->GetDefaultObject());
		if (!DefaultObj) continue;
		if (DefaultObj->IsPersistant) {
			UBaseState* TempState = DuplicateObject(DefaultObj, this);
			TempState->Init(this->CompContext, this);
			this->PersistantStates.Add(TempState);
		}
	}
}

void UCharacterStatesSystem::AnimEventsHandler(EAnimEvt InAnimEvent)
{
	switch (InAnimEvent) {
		case EAnimEvt::ACTIVATE_COMBO:
			this->CrossStateData.IsComboActive = true;
			break;
		case EAnimEvt::DISABLE_COMBO:
			this->CrossStateData.IsComboActive = false;
			break;
		case EAnimEvt::ACTIVATE_INTERRUPTION:
			this->CrossStateData.IsInterruptable = true;
			break;
		case EAnimEvt::DISABLE_INTERRUPTION:
			this->CrossStateData.IsInterruptable = false;
			break;
		default:
			break;
	}
}

void UCharacterStatesSystem::CurrentActionHandler(EActionList Action, EInputEvent InputEvent)
{
	if (!this->CurrentState) return;
	this->CurrentState->ActionHandler(Action, InputEvent);
}

void UCharacterStatesSystem::CurrentAxisHandler(EActionList Action, float AxisValue)
{
	if (!this->CurrentState) return;
	this->CurrentState->AxisHandler(Action, AxisValue);
}

void UCharacterStatesSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!this->CurrentState) return;
	this->CurrentState->Tick(DeltaTime);
}

void UCharacterStatesSystem::ChangeState(FGameplayTag ChangeTo, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	if (this->BlockedTags.HasTagExact(ChangeTo)) return;

	UBaseState* ChangeToObj = nullptr;

	//Get persistant state if available
	for (UBaseState* State : this->PersistantStates)
	{
		if (ChangeTo.MatchesAnyExact(State->ActionTag)) {
			ChangeToObj = State;
			break;
		}
	}

	if (!ChangeToObj) {
		for (TSubclassOf<UBaseState> StateClass : this->GrantedActions) {
			UBaseState* DefaultObj = Cast<UBaseState>(StateClass->GetDefaultObject());
			if (!DefaultObj) continue;
			if (ChangeTo.MatchesAnyExact(DefaultObj->ActionTag)) {
				UBaseState* TempState = DuplicateObject(DefaultObj, this);
				TempState->Init(this->CompContext, this);
				ChangeToObj = TempState;
				break;
			}
		}
	}


	if (!ChangeToObj || !ChangeToObj->StateValidation()) return;

	FGameplayTagContainer From = FGameplayTagContainer();
	if (this->CurrentState) {
		this->CurrentState->OnStateExit();
		From = this->CurrentState->ActionTag;
	}

	this->CurrentState = ChangeToObj;
	this->BlockedTags = this->CurrentState->BlockedTag;
	this->CurrentState->OnStateEnter(From, NewEnterAction, NewEnterEvent);

	/*
	if (!this->IsPlayerControlled() && this->AIController) {
		this->AIController->SetBBCharacterState(ChangeTo);
	} 
	*/
}

void UCharacterStatesSystem::OnHit(AActor* HitInstigator, FDamageInfo InDamageInfo)
{

	if (this->CurrentState->ActionTag.HasTag(FGameplayTag::RequestGameplayTag("ActionStates.Block")) 
		&& UVectorMathLib::CheckBlockDirection(HitInstigator->GetActorLocation(), this->CompContext.CharacterActor->GetActorLocation(), this->CompContext.CharacterActor->GetActorForwardVector())) {
		GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "Blocked");
		InDamageInfo.IsAbsorbed = true;
		this->BlockHitDelegate.Broadcast(HitInstigator, InDamageInfo);
	}
	else {
		GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "OnHit");
		this->TrueHitDelegate.Broadcast(HitInstigator, InDamageInfo);
	}

	if (this->CompContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectReceiveHit(this->CompContext.CharacterActor, HitInstigator, InDamageInfo);
	}

}
