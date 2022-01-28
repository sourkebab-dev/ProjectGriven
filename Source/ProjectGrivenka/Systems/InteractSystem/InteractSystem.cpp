// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractSystem.h"
#include "Components/BoxComponent.h"
#include "ProjectGrivenka/Interfaces/Interactable.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

void UInteractSystem::Init_Implementation()
{
	Super::Init_Implementation();
	if (!this->CompContext->EventBus) {
		UE_LOG(LogTemp, Error, TEXT("Eventbus Initiation Failure"), *GetNameSafe(this)); return;
	}
	if (!this->CompContext->CharacterActor->Implements<UInteractable>()) {
		UE_LOG(LogTemp, Error, TEXT("InteractSystem Initiation Failure"), *GetNameSafe(this)); return;
	}
	UBoxComponent* InteractCollider = IInteractable::Execute_GetInteractionCollider(this->CompContext->CharacterActor);
	if (!InteractCollider) {
		UE_LOG(LogTemp, Error, TEXT("InteractSystem Collider Initiation Failure"), *GetNameSafe(this)); return;
	}
	InteractCollider->OnComponentBeginOverlap.AddDynamic(this, &UInteractSystem::SaveInteractable);
	InteractCollider->OnComponentEndOverlap.AddDynamic(this, &UInteractSystem::RemoveInteractable);

}

void UInteractSystem::SaveInteractable(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//SPONGE: need to check if player is looking at item
	if (!OtherActor->Implements<UInteractable>()|| !IInteractable::Execute_CheckIsInteractable(OtherActor, this->CompContext->CharacterActor) || OtherActor == this->CompContext->CharacterActor) return;
	this->InteractableItem = OtherActor;
}

void UInteractSystem::RemoveInteractable(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->Implements<UInteractable>()) return;
	if (OtherActor == this->InteractableItem) {
		this->InteractableItem = nullptr;
	}
}

AActor* UInteractSystem::GetInteractableItem()
{
	return this->InteractableItem;
}

void UInteractSystem::Interact(AActor* InteractInstigator)
{
	if (!this->CompContext->CharacterActor->Implements<UInteractable>()) {
		UE_LOG(LogTemp, Error, TEXT("InteractSystem Initiation Failure"), *GetNameSafe(this)); return;
	}
	bool IsInteractable = IInteractable::Execute_CheckIsInteractable(this->CompContext->CharacterActor, InteractInstigator);
	if (IsInteractable) {
		this->OnInteract.Broadcast(InteractInstigator);
	}
	else {
		//sponge: maybe i should create a blueprintassignable if an item is unable to be interacted with
	}
}
