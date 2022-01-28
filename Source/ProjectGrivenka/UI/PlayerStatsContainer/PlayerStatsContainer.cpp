// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsContainer.h"
#include "UIHealthBar.h"
#include "UIStaminaBar.h"

void UPlayerStatsContainer::Setup()
{
	APawn* PlayerPawn = this->GetWorld()->GetFirstPlayerController()->GetPawn();
	this->Healthbar->Setup(PlayerPawn);
	this->StaminaBar->Setup(PlayerPawn);
}

void UPlayerStatsContainer::Remove()
{
	this->Healthbar->OnRemove();
	this->StaminaBar->OnRemove();
}
