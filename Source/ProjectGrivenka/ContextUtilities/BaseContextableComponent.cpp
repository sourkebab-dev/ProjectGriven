// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseContextableComponent.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "EventBus.h"

// Sets default values for this component's properties
UBaseContextableComponent::UBaseContextableComponent() : UActorComponent()
{
	this->bWantsInitializeComponent = true;
}

void UBaseContextableComponent::Init_Implementation()
{
	if (!this->GetOwner()->Implements<UContextAvailable>()) {
		GLog->Log(this->GetOwner()->GetFullName());
		UE_LOG(LogTemp, Error, TEXT("Context Initiation Failure"), *GetNameSafe(this)); 
		return;
	}
	
	this->CompContext = IContextAvailable::Execute_GetContext(this->GetOwner());
	if (!this->CompContext) {
		GLog->Log(this->GetOwner()->GetFullName());
		UE_LOG(LogTemp, Error, TEXT("Context Initiated yet not set"), *GetNameSafe(this));
		return;
	}

	if (!this->CompContext->EventBus) {
		GLog->Log(this->GetOwner()->GetFullName());
		UE_LOG(LogTemp, Error, TEXT("Event Bus Initiation Failure"), *GetNameSafe(this));
		return;
	}
	
	
	this->CompContext->EventBus->GenericDelegate.AddDynamic(this, &UBaseContextableComponent::InternalEventHandler);

	GLog->Log(this->GetOwner()->GetFullName());
	UE_LOG(LogTemp, Error, TEXT("Context set"), *GetNameSafe(this));

	
}

void UBaseContextableComponent::Mounted_Implementation()
{

}

void UBaseContextableComponent::InternalEventHandler(EGenericEvt InEvt)
{
	if (InEvt != EGenericEvt::GEN_ALL_COMP_MOUNTED) return;
	this->Mounted_Implementation();
}


