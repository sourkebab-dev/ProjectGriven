// Fill out your copyright notice in the Description page of Project Settings.


#include "UIEquipmentBoxContainer.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridSlot.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "Components/UniformGridPanel.h"

void UUIEquipmentBoxContainer::DataSetup(TSubclassOf<UUIEquipmentItem> EquipmentItemClass)
{
	GLog->Log("EQBSetup");
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return;



	for (int i = 0; i < this->SlotNumber; i++) {
		UUIEquipmentItem* TempItem = this->WidgetTree->ConstructWidget<UUIEquipmentItem>(EquipmentItemClass);
		UUniformGridSlot* GrdSlot = this->EquipmentGrid->AddChildToUniformGrid(TempItem, i / this->ColumnNumber, i % this->ColumnNumber);
		GrdSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		GrdSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	

	for (int i = 0; i < GameInstance->EquipmentBox.Num(); i++) {
		FEquipmentBoxItem Item = GameInstance->EquipmentBox[i];
		if (!this->EquipmentGrid->GetChildAt(Item.BoxIndex)) { GLog->Log("Grid Child Invalid"); continue; };
		UUIEquipmentItem* TempItem = Cast<UUIEquipmentItem>(this->EquipmentGrid->GetChildAt(Item.BoxIndex));
		if (!TempItem) { GLog->Log("UI Class Invalid"); continue; }
		GLog->Log("Set Slot");
		TempItem->SetSlotInfo(Item);
	}
	
}