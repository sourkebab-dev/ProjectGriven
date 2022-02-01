// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameInstance.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Items/ItemPrefab.h"
#include "UIManager.h"

void UBaseGameInstance::Init()
{
    Super::Init();
    //Note: using duplicate object from CDO because for some reason, class defaults doesn't work for uobject initialization (?)
    this->UIManager = Cast<UUIManager>(DuplicateObject(this->UIManagerClass->GetDefaultObject(), this));
    //this->UIManager = NewObject<UUIManager>(this->UIManagerClass);
    this->UIManager->Init(this);
}

FBountyContract UBaseGameInstance::GetAcceptedBountyContract()
{
    UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
    if (!AssetsData) return FBountyContract();


    if (this->AcceptedBountyContractId.IsNone()) return FBountyContract();
    for (int i = 0; i < AssetsData->BountyList->BountyList.Num(); i++) {
        if (this->AcceptedBountyContractId == AssetsData->BountyList->BountyList[i].BountyQuestID) return AssetsData->BountyList->BountyList[i];
    }

    return FBountyContract();
}

void UBaseGameInstance::StoreMaterial(FItemInfo MaterialInfo, int Count)
{
    UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
    UItemPrefab* MaterialPrefab = AssetsData->MaterialMasterData->ItemAssets.FindRef(MaterialInfo.ItemId);
    if (!MaterialPrefab) return;

    int MaxStack = MaterialPrefab->ItemInfo.MaxStackAmount;
    int CountRemain = Count;
    bool isFillNewSlot = true;
    TMap<int, bool> SlotFilledMap;
    for (int i = 0; i < this->MaterialBox.Num(); i++) {
        SlotFilledMap.Add(this->MaterialBox[i].SlotIndex, true);
        if (this->MaterialBox[i].ItemId == MaterialInfo.ItemId) {
            int TempRemain = CountRemain - (MaxStack - this->MaterialBox[i].Count);
            int ToFill = TempRemain < 0 ? CountRemain  : CountRemain - TempRemain;
            this->MaterialBox[i].Count += ToFill;
            CountRemain = TempRemain;

            if (CountRemain <= 0) {
                isFillNewSlot = false;
                break;
            }
        }
    }
    
    if (!isFillNewSlot) return;

    int TempSlotIdx = 0;
    FPersistedInventoryItems PersistedInfo;
    PersistedInfo.ItemId = MaterialInfo.ItemId;

    while (CountRemain > 0) {
        if (SlotFilledMap.Find(TempSlotIdx)) {
            TempSlotIdx++;
            continue;
        }

        int TempRemain = CountRemain - MaxStack;
        int ToFill = TempRemain < 0 ? CountRemain : MaxStack;
        PersistedInfo.Count = ToFill;
        PersistedInfo.SlotIndex = TempSlotIdx;
        this->MaterialBox.Add(PersistedInfo);
        CountRemain = TempRemain;
    }
}

void UBaseGameInstance::DepleteMaterial(FItemInfo MaterialInfo, int Count)
{
}

bool UBaseGameInstance::CheckSmithingRequirements(FItemInfo MaterialInfo, int Count)
{
    return true;
}
