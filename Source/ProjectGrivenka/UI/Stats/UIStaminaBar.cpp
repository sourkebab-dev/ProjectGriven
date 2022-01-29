// Fill out your copyright notice in the Description page of Project Settings.


#include "UIStaminaBar.h"
#include "Components/ProgressBar.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"

void UUIStaminaBar::Setup(AActor* InOwner)
{
	this->Owner = InOwner;
	//sponge: need to unbind
	this->OnRender.BindUFunction(this, "Render");
	ICharacterSystemAvailable::Execute_SubscribeAttributeChanges(InOwner, EAttributeCode::ATT_Stamina, this->OnRender);
	this->Render();
}

void UUIStaminaBar::Render()
{
	float MaxStamina = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(this->Owner, EAttributeCode::ATT_Stamina);
	float Stamina = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->Owner, EAttributeCode::ATT_Stamina);
	this->Bar->SetPercent(Stamina / MaxStamina);
}

void UUIStaminaBar::OnRemove()
{
	if (!this->Owner) return;
	ICharacterSystemAvailable::Execute_UnSubscribeAttributeChanges(this->Owner, EAttributeCode::ATT_Stamina, this->OnRender);
}
