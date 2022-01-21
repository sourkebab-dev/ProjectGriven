// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Interfaces/Interactable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

//Sponge need to implement usage type
//sponge: maybe save interactable item in member?

bool UInteractState::StateValidation_Implementation()
{
	if (!this->StatesComp->CompContext->CharacterActor->Implements<UInteractable>() || !IInteractable::Execute_GetInteractableItem(this->StatesComp->CompContext->CharacterActor)) return false;
	return true;
}


void UInteractState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
}

void UInteractState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
	if (this->InteractionInfo.LocomotionType == EInteractablesLocomotionType::INL_BLOCK) return;
	if (Action == EActionList::ActionMoveForward || Action == EActionList::ActionMoveRight)
	{
		this->StatesComp->CompContext->MovementComp->AddInputVector(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir);
	}
}

void UInteractState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	AActor* InteractableItem = IInteractable::Execute_GetInteractableItem(this->StatesComp->CompContext->CharacterActor);
	if (!InteractableItem) 	this->EndInteraction();
	IInteractable::Execute_RequestInteractionInfo(InteractableItem, this->StatesComp->CompContext->CharacterActor, this->InteractionInfo);

	FOnMontageEnded EndInteractDelegate;
	//Note: CDL_Interact will be triggered by notify states for latent interaction
	if (this->InteractionInfo.IsLatent) {
		this->StatesComp->CompContext->EventBus->InteractionDelegate.AddDynamic(this, &UInteractState::StartInteraction);
	}
	else {
		this->StartInteraction();
		if (!this->InteractionInfo.InteractionAnim) {
			this->EndInteraction(); 
			return; 
		}
	}

	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->InteractionInfo.InteractionAnim);
	EndInteractDelegate.BindUFunction(this, "EndInteraction");
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndInteractDelegate, this->InteractionInfo.InteractionAnim);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndInteractDelegate, this->InteractionInfo.InteractionAnim);
}

void UInteractState::OnStateExit_Implementation()
{
	this->InteractionInfo = FInteractionInfo();
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.25);

	if(this->InteractionInfo.IsLatent) {
		this->StatesComp->CompContext->EventBus->InteractionDelegate.RemoveDynamic(this, &UInteractState::StartInteraction);
	}
}

void UInteractState::StartInteraction()
{
	AActor* InteractableItem = IInteractable::Execute_GetInteractableItem(this->StatesComp->CompContext->CharacterActor);
	if (!InteractableItem) { this->EndInteraction(); return; }
	IInteractable::Execute_Interact(InteractableItem, this->StatesComp->CompContext->CharacterActor);
}

void UInteractState::EndInteraction()
{
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
}