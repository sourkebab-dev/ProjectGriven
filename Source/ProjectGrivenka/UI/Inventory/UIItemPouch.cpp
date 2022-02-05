// Fill out your copyright notice in the Description page of Project Settings.


#include "UIItemPouch.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/WidgetTree.h"
#include "UIItemPouchSlot.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"

void UUIItemPouch::Render(AActor* Owner)
{
	this->PouchGrid->ClearChildren();

	for (int i = 0; i < this->SlotNumber; i++) {
		auto TempItem = this->WidgetTree->ConstructWidget<UUIItemPouchSlot>(this->ItemPouchSlotClass);
		UUniformGridSlot* GrdSlot = this->PouchGrid->AddChildToUniformGrid(TempItem, i / this->ColumnNumber, i % this->ColumnNumber);
		GrdSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		GrdSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		TempItem->ItemInfo.SlotIndex = i;
		TempItem->Owner = Owner;
	}

	if (!Owner->Implements<UCharacterInventoryAvailable>()) return;

	FPersistedInventory OwnerInvData = ICharacterInventoryAvailable::Execute_GetInventoryData(Owner);
	for (int i = 0; i < OwnerInvData.Items.Num(); i++) {
		FPersistedInventoryItems Item = OwnerInvData.Items[i];
		if (!this->PouchGrid->GetChildAt(Item.SlotIndex)) { GLog->Log("Grid Child Invalid"); continue; };
		auto TempItem = Cast<UUIItemPouchSlot>(this->PouchGrid->GetChildAt(Item.SlotIndex));
		if (!TempItem) { GLog->Log("UI Class Invalid"); continue; }
		TempItem->Render(Owner, Item);
	}

}
