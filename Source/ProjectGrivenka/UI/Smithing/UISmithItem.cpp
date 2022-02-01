// Fill out your copyright notice in the Description page of Project Settings.


#include "UISmithItem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabs.h"
#include "ProjectGrivenka/Equipments/SmithingRecipePrefab.h"

void UUISmithItem::RenderInfo()
{
	if (!this->RecipePrefab) return;
	this->SmithImage->SetBrushFromTexture(this->RecipePrefab->SmithingResultWeapon->WeaponInfo.GeneralInfo.EquipmentIcon);
}

void UUISmithItem::Disable()
{
	this->SmithButton->SetIsEnabled(false);
}

void UUISmithItem::Enable()
{
	this->SmithButton->SetIsEnabled(true);
	//Sponge: image opacity
	//this->SmithImage->SetBrushTintColor(FSlateColor::GetColor());
}
