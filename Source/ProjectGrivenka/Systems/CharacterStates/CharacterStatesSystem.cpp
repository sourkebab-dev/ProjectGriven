// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatesSystem.h"
#include "BaseState.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

UCharacterStatesSystem::UCharacterStatesSystem() : UBaseContextableComponent() {
	this->PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterStatesSystem::Init()
{
	Super::Init();
	this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	if (!this->CompContext.EventBus) return;
	this->CompContext.EventBus->StateActionDelegate.AddDynamic(this, &UCharacterStatesSystem::CurrentActionHandler);
	this->CompContext.EventBus->StateAxisDelegate.AddDynamic(this, &UCharacterStatesSystem::CurrentAxisHandler);
	this->CompContext.EventBus->AnimDelegate.AddDynamic(this, &UCharacterStatesSystem::AnimEventsHandler);
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

void UCharacterStatesSystem::AssignStateByTag(FGameplayTag InTag, UBaseState* OutState)
{
	for (TSubclassOf<UBaseState> StateClass : this->GrantedActions) {
		UBaseState* DefaultObj = Cast<UBaseState>(StateClass->GetDefaultObject());
		if (!DefaultObj) continue;
		if (InTag.MatchesAnyExact(DefaultObj->ActionTag)) {
			UBaseState* TempState = DuplicateObject(DefaultObj, this);
			TempState->Init(this->CompContext, this);
			OutState = TempState;
		}
	}
}

void UCharacterStatesSystem::ChangeState(FGameplayTag ChangeTo, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	if (this->BlockedTags.HasTagExact(ChangeTo)) return;

	UBaseState* ChangeToObj = nullptr;

	for (TSubclassOf<UBaseState> StateClass : this->GrantedActions) {
		UBaseState* DefaultObj = Cast<UBaseState>(StateClass->GetDefaultObject());
		if (!DefaultObj) continue;
		if (ChangeTo.MatchesAnyExact(DefaultObj->ActionTag)) {
			UBaseState* TempState = DuplicateObject(DefaultObj, this);
			TempState->Init(this->CompContext, this);
			ChangeToObj = TempState;
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
