// Fill out your copyright notice in the Description page of Project Settings.


#include "UINPCStatsContainer.h"
#include "UIHealthBar.h"
#include "UIStaminaBar.h"

//sponge: maybe merge with playerstatscontainer ?
void UUINPCStatsContainer::Setup(AActor* InOwner)
{
	this->Healthbar->Setup(InOwner);
	this->StaminaBar->Setup(InOwner);
}

void UUINPCStatsContainer::Remove()
{
	this->Healthbar->OnRemove();
	this->StaminaBar->OnRemove();
}
