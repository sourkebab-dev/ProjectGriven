// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameInstance.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
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
