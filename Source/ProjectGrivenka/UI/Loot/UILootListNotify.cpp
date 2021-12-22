// Fill out your copyright notice in the Description page of Project Settings.


#include "UILootListNotify.h"
#include "Components/Image.h"
#include "ProjectGrivenka/Items/ItemPrefab.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"
#include "Components/TextBlock.h"

void UUILootListNotify::DequeueLootDisplay()
{
	if (this->LootQueue.IsEmpty()) {
		this->GetWorld()->GetTimerManager().ClearTimer(this->QueueTimer);
		UBaseGameInstance* GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
		GI->UIManager->RemoveLootDisplay();
		return;
	}
	FLootItem DisplayedLoot;
	this->LootQueue.Dequeue(DisplayedLoot);
	this->SetItemInfo(DisplayedLoot);
}

void UUILootListNotify::EnqueueLootDisplay(FLootItem InLootItem)
{
	this->LootQueue.Enqueue(InLootItem);
	if (!this->GetWorld()->GetTimerManager().IsTimerActive(this->QueueTimer)) {
		this->GetWorld()->GetTimerManager().SetTimer(this->QueueTimer, this, &UUILootListNotify::DequeueLootDisplay, 2.5f, true, 0.0f);
	}
}

void UUILootListNotify::SetItemInfo(FLootItem InLootItem)
{
	this->LootIcon->SetBrushFromTexture(InLootItem.Item->ItemInfo.ItemIcon);
	this->LootName->SetText(FText::FromName(InLootItem.Item->ItemInfo.ItemName));
	this->LootNumber->SetText(FText::FromString(FString::FromInt(InLootItem.Quantity)));
}
