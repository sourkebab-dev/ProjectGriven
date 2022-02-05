// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventory.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Items/BaseItem.h"
#include "ProjectGrivenka/Items/ItemPrefab.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"


void UCharacterInventory::Init_Implementation()
{
	Super::Init_Implementation();
}

void UCharacterInventory::SaveData()
{

}

void UCharacterInventory::LoadData(FPersistedInventory InInventory)
{
	this->InventoryList = InInventory;
}

void UCharacterInventory::SetSelectedItem(int Idx)
{
	int oldIdx = this->SelectedItemBeltIdx;
	if (this->ItemBelt.Num() == 0 || Idx >= this->ItemBelt.Num()) this->SelectedItemBeltIdx = 0;
	else if (Idx < 0) this->SelectedItemBeltIdx = this->ItemBelt.Num() - 1;
	else this->SelectedItemBeltIdx = Idx; 
	

	if (this->SelectedItemBeltIdx != oldIdx) {
		//Sponge: should probably check whether char controlled by player or not
		this->SyncItemBeltUI();
	}

}

UItemPrefab* UCharacterInventory::GetSelectedItemInventory()
{
	if (!this->ItemBelt.IsValidIndex(this->SelectedItemBeltIdx)) return nullptr;
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	int InventoryIdx = this->ItemBelt[this->SelectedItemBeltIdx];
	if (!this->InventoryList.Items.IsValidIndex(InventoryIdx)) return nullptr;
	UItemPrefab* ItemPrefab = AssetsData->ItemPrefab->ItemAssets.FindRef(this->InventoryList.Items[InventoryIdx].ItemId);
	return ItemPrefab;
}

void UCharacterInventory::UseSelectedItemBelt()
{
	if (!this->ItemBelt.IsValidIndex(this->CommitedItemBeltIdx)) return;
	int InventoryIdx = this->ItemBelt[this->CommitedItemBeltIdx];
	if (!this->InventoryList.Items.IsValidIndex(InventoryIdx)) return;
	this->AddToStack(InventoryIdx, -1);
	this->UsedItem->OnUse();
	this->SaveData();
}

void UCharacterInventory::CommitItem()
{
	if (this->UsedItem) this->UsedItem->Destroy();
	UItemPrefab* SelectedItemPrefab = this->GetSelectedItemInventory();
	if (SelectedItemPrefab) {
		int InventoryIdx = this->ItemBelt[this->SelectedItemBeltIdx];

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParameters.Owner = this->GetOwner();
		if (!this->GetOwner()) return;
		this->CommitedItemBeltIdx = this->SelectedItemBeltIdx;
		this->UsedItem = this->CompContext->CharacterActor->GetWorld()->SpawnActor<ABaseItem>(SelectedItemPrefab->ItemInfo.ItemBaseClass, SpawnParameters);
		this->UsedItem->LoadInfo(SelectedItemPrefab->ItemInfo);

		//SPONGE: move item to hand ?
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		this->UsedItem->AttachToComponent(this->CompContext->SkeletalMeshComp, TransformRules, "WeaponSlot");

	}
	else {
		this->SelectedItemBeltIdx = 0;
		this->CommitedItemBeltIdx = 0;
	}
}

void UCharacterInventory::AddItemToInventory(FName InItemId, int InItemCount)
{
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UItemPrefab* ItemPrefab = AssetsData->ItemPrefab->ItemAssets.FindRef(InItemId);
	if (!ItemPrefab) return;

	int MaxStack = ItemPrefab->ItemInfo.MaxStackAmount;
	int CountRemain = InItemCount;
	bool isFillNewSlot = true;

	//Sponge: maybe i should cache this slots filled data?
	TMap<int, bool> SlotFilledMap;
	for (int i = 0; i < this->InventoryList.Items.Num(); i++) {
		SlotFilledMap.Add(this->InventoryList.Items[i].SlotIndex, true);
		if (this->InventoryList.Items[i].ItemId == InItemId) {
			int TempRemain = CountRemain - (MaxStack - this->InventoryList.Items[i].Count);
			int ToFill = TempRemain < 0 ? CountRemain : CountRemain - TempRemain;
			this->AddToStack(i, ToFill);
			CountRemain = TempRemain;

			if (CountRemain <= 0) {
				isFillNewSlot = false;
				break;
			}
		}
	}

	if (!isFillNewSlot) return;
	if (this->InventoryList.Items.Num() >= 30) {
		GEngine->AddOnScreenDebugMessage(FMath::Rand(), 2, FColor::Yellow, "Inventory Full");
		return;
	}


	int TempSlotIdx = 0;
	FPersistedInventoryItems PersistedInfo;
	PersistedInfo.ItemId = InItemId;

	while (CountRemain > 0) {
		if (SlotFilledMap.Find(TempSlotIdx)) {
			TempSlotIdx++;
			continue;
		}

		int TempRemain = CountRemain - MaxStack;
		int ToFill = TempRemain < 0 ? CountRemain : MaxStack;
		PersistedInfo.Count = ToFill;
		PersistedInfo.SlotIndex = TempSlotIdx;
		this->InventoryList.Items.Add(PersistedInfo);
		CountRemain = TempRemain;
	}

	this->SaveData();
}

void UCharacterInventory::AddToStack(int InInventoryIndex, int InToFill)
{
	this->InventoryList.Items[InInventoryIndex].Count += InToFill;
	int ItemBeltIndex = this->ItemBelt.Find(InInventoryIndex);
	if (ItemBeltIndex == INDEX_NONE) return;
	if (this->InventoryList.Items[InInventoryIndex].Count <= 0) {
		this->ItemBelt.RemoveAt(ItemBeltIndex);
		this->SetSelectedItem(ItemBeltIndex + 1);
	}
	this->SyncItemBeltUI();
}

void UCharacterInventory::SyncItemBeltUI()
{
	UItemPrefab* ItemPrefab = this->GetSelectedItemInventory();
	if (!ItemPrefab) return;
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	GameInstance->UIManager->SetActiveItemImage(ItemPrefab->ItemInfo);
}

void UCharacterInventory::ToggleShowInventory()
{
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 3, FColor::Cyan, "Ontoggle");

	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (this->IsUIShown) {
		GameInstance->UIManager->CloseInventoryUI(this->CompContext->CharacterActor);
	}
	else {
		GameInstance->UIManager->OpenInventoryUI(this->CompContext->CharacterActor);
	}
	this->IsUIShown = !this->IsUIShown;
}

void UCharacterInventory::MoveSlot(FName ItemId, int SourceIndex, int NewSourceIndex)
{
	GLog->Log(ItemId.ToString());
	GLog->Log(FString::FromInt(SourceIndex));
	GLog->Log(FString::FromInt(NewSourceIndex));

	for (int i = 0; i < this->InventoryList.Items.Num(); i++) {
		if (this->InventoryList.Items[i].SlotIndex == SourceIndex && this->InventoryList.Items[i].ItemId == ItemId) {
			this->InventoryList.Items[i].SlotIndex = NewSourceIndex;
			break;
		}
	}
	this->SaveData();
}
