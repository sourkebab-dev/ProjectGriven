// Fill out your copyright notice in the Description page of Project Settings.


#include "UIItemPouchSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

void UUIItemPouchSlot::Render(AActor* InOwner, FPersistedInventoryItems InItemInfo)
{
	//Sponge: it's probably not good to store a reference to owner like this
	this->Owner = InOwner;
	this->ItemInfo = InItemInfo;
	if (InItemInfo.ItemId.IsNone()) {
		this->SlotImage->SetBrushFromTexture(nullptr);
		this->ItemCount->SetText(FText::FromString(""));
		return;
	}

	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	auto ItemPrefab = AssetsData->ItemPrefab->ItemAssets.FindRef(InItemInfo.ItemId);
	if (!ItemPrefab) return;

	this->SlotImage->SetBrushFromTexture(ItemPrefab->ItemInfo.ItemIcon);
	this->ItemCount->SetText(FText::FromString(FString::FromInt(InItemInfo.Count)));
}
