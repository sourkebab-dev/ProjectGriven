// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventory.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGrivenka/Items/BaseItem.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"


void UCharacterInventory::Init()
{
	Super::Init();	
}

void UCharacterInventory::SyncItemBeltUI()
{
	int InventoryIdx = this->ItemBelt[this->SelectedItemBeltIdx];
	if (this->ItemBelt.IsValidIndex(this->SelectedItemBeltIdx) && this->InventoryList.IsValidIndex(this->ItemBelt[this->SelectedItemBeltIdx])) {
		UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
		UItemPrefab* ItemPrefab = AssetsData->ItemPrefab->ItemAssets.FindRef(this->InventoryList[InventoryIdx].ItemId);
		if (!ItemPrefab) return;
		FItemInfo ItemInfo = ItemPrefab->ItemInfo;
		UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
		GameInstance->UIManager->SetActiveItemImage(ItemInfo);
	}

}

void UCharacterInventory::SetSelectedItem(int Idx)
{
	int oldIdx = this->SelectedItemBeltIdx;
	if (this->ItemBelt.Num() == 0 || Idx >= this->ItemBelt.Num()) this->SelectedItemBeltIdx = 0;
	else if (Idx < 0) this->SelectedItemBeltIdx = this->ItemBelt.Num() - 1;
	else this->SelectedItemBeltIdx = Idx; 
	

	//SPONGE: DELETE LATER
	if (this->SelectedItemBeltIdx != oldIdx) {
		//Sponge: should probably check whether char controlled by player or not
		this->SyncItemBeltUI();
	}

}

FPersisted_CharacterItems UCharacterInventory::GetSelectedItemInventory()
{
	if (!this->ItemBelt.IsValidIndex(this->SelectedItemBeltIdx)) return FPersisted_CharacterItems();
	int InventoryIdx = this->ItemBelt[this->SelectedItemBeltIdx];
	return this->InventoryList.IsValidIndex(InventoryIdx) ? this->InventoryList[InventoryIdx] : FPersisted_CharacterItems();
}

void UCharacterInventory::UseSelectedItemBelt()
{
	if (!this->ItemBelt.IsValidIndex(this->CommitedItemBeltIdx)) return;
	int InventoryIdx = this->ItemBelt[this->CommitedItemBeltIdx];
	if (!this->InventoryList.IsValidIndex(InventoryIdx)) return;
	this->UsedItem->OnUse();
	this->InventoryList[InventoryIdx].Count -= 1;
}

void UCharacterInventory::CommitItem()
{
	//Sponge: should probably subtract inventory count
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	if (this->UsedItem) this->UsedItem->Destroy();
	if (this->ItemBelt.IsValidIndex(this->SelectedItemBeltIdx) && this->InventoryList.IsValidIndex(this->ItemBelt[this->SelectedItemBeltIdx])) {
		int InventoryIdx = this->ItemBelt[this->SelectedItemBeltIdx];
		UItemPrefab* ItemPrefab = AssetsData->ItemPrefab->ItemAssets.FindRef(this->InventoryList[InventoryIdx].ItemId);
		if (!ItemPrefab) return;
		FItemInfo ItemInfo = ItemPrefab->ItemInfo;
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParameters.Owner = this->GetOwner();
		if (!this->GetOwner()) return;
		this->CommitedItemBeltIdx = this->SelectedItemBeltIdx;
		this->UsedItem = this->CompContext.CharacterActor->GetWorld()->SpawnActor<ABaseItem>(ItemInfo.ItemBaseClass, SpawnParameters);
		this->UsedItem->LoadInfo(ItemInfo);

		//SPONGE: move item to hand ?
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		this->UsedItem->AttachToComponent(this->CompContext.SkeletalMeshComp, TransformRules, "WeaponSlot");
	}
	else {
		this->SelectedItemBeltIdx = 0;
		this->CommitedItemBeltIdx = 0;
	}
}

int UCharacterInventory::GetAvailableSlotIdx(FName InItemId, int InItemCount)
{
	//sponge: should add stacking feature
	if (this->InventoryList.Num() == 40) { GLog->Log("Inventory Full"); return -1; }
	int AvailableIndex = 0;
	int MaxSlotIdx = 0;
	TMap<int, FPersisted_CharacterItems> InventoryMap;
	for (int i = 0; i < this->InventoryList.Num(); i++) {
		InventoryMap.Add(this->InventoryList[i].SlotIndex, this->InventoryList[i]);
		if (MaxSlotIdx < this->InventoryList[i].SlotIndex) {
			MaxSlotIdx = this->InventoryList[i].SlotIndex;
		}
	}

	for (int i = 0; i <= MaxSlotIdx + 1; i++) {
		if (InventoryMap.FindRef(i).ItemId.IsNone()) {
			AvailableIndex = i;
			break;
		}
	}

	return AvailableIndex;
}

void UCharacterInventory::AddItemToInventory(FName InItemId, int InItemCount)
{
	int AvailableIndex = this->GetAvailableSlotIdx(InItemId, InItemCount);
	if (AvailableIndex < 0) return;
	FPersisted_CharacterItems InvSlotInfo;
	InvSlotInfo.SlotIndex = AvailableIndex;
	InvSlotInfo.ItemId = InItemId;
	InvSlotInfo.Count = InItemCount;
	int NewInvIdx = this->InventoryList.Add(InvSlotInfo);
	this->ItemBelt.Add(NewInvIdx);
}
