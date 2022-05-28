// Fill out your copyright notice in the Description page of Project Settings.


#include "UIEquipmentBoxContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridSlot.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"

void UUIEquipmentBoxContainer::NativeConstruct()
{
	this->ArmorTabButton->OnClicked.AddDynamic(this, &UUIEquipmentBoxContainer::OnArmorTabClick);
	this->WeaponTabButton->OnClicked.AddDynamic(this, &UUIEquipmentBoxContainer::OnWeaponTabClick);
}

void UUIEquipmentBoxContainer::NativeDestruct()
{
	this->ArmorTabButton->OnClicked.RemoveDynamic(this, &UUIEquipmentBoxContainer::OnArmorTabClick);
	this->WeaponTabButton->OnClicked.RemoveDynamic(this, &UUIEquipmentBoxContainer::OnWeaponTabClick);
}

void UUIEquipmentBoxContainer::OnArmorTabClick()
{
	//sponge craft armor
	//if (this->CurrentEqType == EEquipmentType::Armor) return;
	//this->CurrentEqType = EEquipmentType::Armor;
	//this->DataSetup();
}

void UUIEquipmentBoxContainer::OnWeaponTabClick()
{
	if (this->CurrentEqType == EEquipmentType::Weapon) return;
	this->CurrentEqType = EEquipmentType::Weapon;
	this->DataSetup();
}

void UUIEquipmentBoxContainer::DataSetup()
{
	GLog->Log("EQBSetup");
	this->EquipmentGrid->ClearChildren();
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return;



	for (int i = 0; i < this->SlotNumber; i++) {
		UUIEquipmentItem* TempItem = this->WidgetTree->ConstructWidget<UUIEquipmentItem>(this->EquipmentItemClass);
		UUniformGridSlot* GrdSlot = this->EquipmentGrid->AddChildToUniformGrid(TempItem, i / this->ColumnNumber, i % this->ColumnNumber);
		GrdSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		GrdSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	
	TArray<FEquipmentBoxItem> SelectedBox;
	if (this->CurrentEqType == EEquipmentType::Weapon) {
		SelectedBox = GameInstance->WeaponBox;
	}
	else {
		SelectedBox = GameInstance->ArmorBox;
	}


	for (int i = 0; i < SelectedBox.Num(); i++) {
		FEquipmentBoxItem Item = SelectedBox[i];
		if (!this->EquipmentGrid->GetChildAt(Item.BoxIndex)) { GLog->Log("Grid Child Invalid"); continue; };
		UUIEquipmentItem* TempItem = Cast<UUIEquipmentItem>(this->EquipmentGrid->GetChildAt(Item.BoxIndex));
		if (!TempItem) { GLog->Log("UI Class Invalid"); continue; }
		GLog->Log("Set Slot");
		TempItem->SetSlotInfo(Item, this->CurrentEqType);
	}
	
}