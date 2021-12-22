// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseContextableComponent.h"
#include "EventBus.h"

// Sets default values for this component's properties
UBaseContextableComponent::UBaseContextableComponent() : UActorComponent()
{
	this->bWantsInitializeComponent = true;
}

void UBaseContextableComponent::Init()
{
	if (!this->GetOwner()->Implements<UContextAvailable>()) {
		GLog->Log(this->GetOwner()->GetFullName());
		UE_LOG(LogTemp, Error, TEXT("Context Initiation Failure"), *GetNameSafe(this)); 
		return;
	}
	
	IContextAvailable::Execute_GetContext(this->GetOwner(), this->CompContext);
}


