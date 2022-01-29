// Fill out your copyright notice in the Description page of Project Settings.


#include "UIHealthBar.h"
#include "Components/ProgressBar.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"

void UUIHealthBar::Setup(AActor* InOwner)
{
	this->Owner = InOwner;
	//sponge: need to unbind
	this->OnRender.BindUFunction(this, "Render");
	ICharacterSystemAvailable::Execute_SubscribeAttributeChanges(InOwner, EAttributeCode::ATT_Health, OnRender);
	this->Render();
}

void UUIHealthBar::Render()
{
	float MaxHealth = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(this->Owner, EAttributeCode::ATT_Health);
	float Health = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->Owner, EAttributeCode::ATT_Health);
	this->Bar->SetPercent(Health / MaxHealth);
}

void UUIHealthBar::OnRemove()
{
	if (!this->Owner) return;
	ICharacterSystemAvailable::Execute_UnSubscribeAttributeChanges(this->Owner, EAttributeCode::ATT_Health, this->OnRender);
}
