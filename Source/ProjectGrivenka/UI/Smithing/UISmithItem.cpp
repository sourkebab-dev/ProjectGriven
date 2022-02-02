// Fill out your copyright notice in the Description page of Project Settings.


#include "UISmithItem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabs.h"
#include "ProjectGrivenka/Equipments/SmithingRecipePrefab.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"


void UUISmithItem::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	this->SmithButton->OnClicked.AddDynamic(this, &UUISmithItem::OnClick);
}

void UUISmithItem::NativeDestruct()
{
	this->SmithButton->OnClicked.RemoveDynamic(this, &UUISmithItem::OnClick);
	Super::NativeDestruct();
}

void UUISmithItem::Render()
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

void UUISmithItem::OnClick()
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	//sponge: need to pass real guid & type
	GI->UIManager->EmitSmithPicked(this->RecipePrefab->SmithingResultWeapon->WeaponInfo.GeneralInfo.EquipmentId);
}
