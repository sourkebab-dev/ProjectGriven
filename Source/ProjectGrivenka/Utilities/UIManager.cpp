// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "Blueprint/UserWidget.h"
#include "BaseGameInstance.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/UI/Dialogue/DialogueBox.h"
#include "ProjectGrivenka/UI/ItemBelt/ActiveItemBelt.h"
#include "ProjectGrivenka/UI/Loot/UILootListNotify.h"
#include "ProjectGrivenka/UI/Stats/PlayerStatsContainer.h"
#include "ProjectGrivenka/UI/Smithing/UISmithTreeWrapper.h"
#include "ProjectGrivenka/UI/EquipmentBox/UIEquipmentBoxContainer.h"
#include "ProjectGrivenka/UI/Inventory/UIInventoryContainer.h"
#include "ProjectGrivenka/Equipments/WeaponPrefabs.h"

void UUIManager::Init(UBaseGameInstance* InGameIns) {
	this->GameIns = InGameIns;
}

void UUIManager::LoadInGameUI()
{
	this->ActiveItemBeltUIIns = Cast<UActiveItemBelt>(CreateWidget(this->GameIns, this->ActiveItemBeltUIClass, "Active Item Belt UI"));
	this->ActiveItemBeltUIIns->AddToViewport();
	this->PlayerStatsContainerUIIns = Cast<UPlayerStatsContainer>(CreateWidget(this->GameIns, this->PlayerStatsContainerUIClass, "Player Stats Container UI"));
	this->PlayerStatsContainerUIIns->AddToViewport();
}

void UUIManager::StatsSetup()
{
	if (!this->PlayerStatsContainerUIIns) return;
	this->PlayerStatsContainerUIIns->Setup();
}

void UUIManager::OpenInventoryUI(AActor* Owner)
{
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameAndUI());
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

	auto UIns = this->InventoryUIIns.FindRef(Owner);
	if (!UIns) {
		 UIns = Cast<UUIInventoryContainer>(CreateWidget(this->GameIns, this->InventoryUIClass, "Inventory UI"));
		 this->InventoryUIIns.Add(Owner, UIns);
	}
	UIns->Render(Owner);
	if (!UIns->IsInViewport()) {
		UIns->AddToViewport();
	}
}

void UUIManager::CloseInventoryUI(AActor* Owner)
{
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 3, FColor::Cyan, "TryClose");
	auto UIns = this->InventoryUIIns.FindRef(Owner);
	if (!UIns) return;
	this->InventoryUIIns.Remove(Owner);
	UIns->RemoveFromViewport();
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
}

void UUIManager::SetActiveItemImage(FItemInfo InItemInfo)
{
	this->ActiveItemBeltUIIns->SetItemInfo(InItemInfo);
}

void UUIManager::EmitUIClose()
{
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

	this->UICloseDelegate.Broadcast();
}

void UUIManager::EmitUIBack()
{
	this->UIBackDelegate.Broadcast();
}

void UUIManager::ShowDialogue(FDialogueData InDialogueData)
{
	if (!this->DialogueBoxUIIns || !this->DialogueBoxUIIns->IsInViewport()) {
		this->OpenDialogueBox();
	}

	this->DialogueBoxUIIns->SetIsAQuestion(false);
	this->DialogueBoxUIIns->SetDialogue(InDialogueData);
}

void UUIManager::ShowDialogueReply(TArray<FReplyData> InReplyData, FDialogueData InDialogueData)
{
	if (!this->DialogueBoxUIIns || !this->DialogueBoxUIIns->IsInViewport()) {
		this->OpenDialogueBox();
	}
	this->DialogueBoxUIIns->SetIsAQuestion(true);
	this->DialogueBoxUIIns->SetDialogue(InDialogueData);
	this->DialogueBoxUIIns->SetReplyData(InReplyData);
	
}

void UUIManager::OpenDialogueBox()
{
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 1, FColor::Cyan, "dunnowhat");

	//Sponge: idunno if i should spawn & despawn ui
	if (!this->DialogueBoxUIIns) {
		this->DialogueBoxUIIns =  Cast<UDialogueBox>(CreateWidget(this->GameIns, this->DialogueBoxUIClass, "Dialogue Box UI"));
	}

	if (this->DialogueBoxUIIns) {
		this->DialogueBoxUIIns->AddToViewport();
	}
}

void UUIManager::CloseDialogueBox()
{
	if (this->DialogueBoxUIIns)
	{
		this->DialogueBoxUIIns->RemoveFromViewport();
	}
}

void UUIManager::EnqueueLootDisplay(FLootItem InLootItem)
{
	if (!this->LootListUIIns) {
		this->LootListUIIns = Cast<UUILootListNotify>(CreateWidget(this->GameIns, this->LootListUIClass, "Loot List UI"));
	}

	if (this->LootListUIIns) {
		if(!this->LootListUIIns->IsInViewport()) this->LootListUIIns->AddToViewport();
		this->LootListUIIns->EnqueueLootDisplay(InLootItem);
	}
}

void UUIManager::RemoveLootDisplay()
{
	if (this->LootListUIIns) this->LootListUIIns->RemoveFromViewport();
}

void UUIManager::EmitEquipmentBoxClicked(FPersistedEquipmentItem EquipmentInfo, EEquipmentType InEquipmentType)
{
	if (!this->GameIns) return;
	this->EquipmentBoxClickedDelegate.Broadcast(EquipmentInfo, InEquipmentType);
}

void UUIManager::EmitProceedDialogue()
{
	this->DialogueProceedDelegate.Broadcast();
}

void UUIManager::EmitReplyDialogue(FName InReplyId)
{
	this->DialogueReplyDelegate.Broadcast(InReplyId);
}

void UUIManager::OpenSmithEQBoxUI()
{
	this->EquipmentBoxClickedDelegate.AddDynamic(this, &UUIManager::OnSmithEQBoxChosen);
	this->UICloseDelegate.AddDynamic(this, &UUIManager::CloseSmithEQBoxUI);

	if (!this->EquipmentBoxUIIns) {
		this->EquipmentBoxUIIns = Cast<UUIEquipmentBoxContainer>(CreateWidget(this->GameIns, this->EquipmentBoxUIClass, "Equipment Box Container UI"));
	}

	if (this->EquipmentBoxUIIns) {
		this->EquipmentBoxUIIns->DataSetup();
		this->EquipmentBoxUIIns->AddToViewport();
	}
}

void UUIManager::CloseSmithEQBoxUI()
{
	this->UICloseDelegate.RemoveDynamic(this, &UUIManager::CloseSmithEQBoxUI);
	this->EquipmentBoxClickedDelegate.RemoveDynamic(this, &UUIManager::OnSmithEQBoxChosen);
	if (this->EquipmentBoxUIIns) {
		this->EquipmentBoxUIIns->RemoveFromViewport();
	}
}

void UUIManager::CloseSmithTree(bool IsBack)
{
	this->UIBackDelegate.RemoveDynamic(this, &UUIManager::OnSmithTreeBack);

	if (this->SmithTreeWrapperUIIns) {
		this->SmithTreeWrapperUIIns->RemoveFromViewport();
	}
	
	if (IsBack) {
		this->OpenSmithEQBoxUI();
	}
}

void UUIManager::OnSmithEQBoxChosen(FPersistedEquipmentItem EquipmentInfo, EEquipmentType InEquipmentType)
{
	this->CloseSmithEQBoxUI();
	this->UIBackDelegate.AddDynamic(this, &UUIManager::OnSmithTreeBack);
	if (!this->SmithTreeWrapperUIIns) {
		this->SmithTreeWrapperUIIns = Cast<UUISmithTreeWrapper>(CreateWidget(this->GameIns, this->SmithTreeWrapperUIClass, "Smith Tree Wrapper UI"));
	}

	if (this->SmithTreeWrapperUIIns) {
		UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
		EEquipmentTree EquipmentTree;
		if (InEquipmentType == EEquipmentType::Weapon) {
			auto WeaponPrefab = AssetsData->WeaponPrefabs->WeaponAssets.FindRef(EquipmentInfo.VariantId);
			EquipmentTree = WeaponPrefab->WeaponInfo.GeneralInfo.EquipmentTree;

		}
		else {
			GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "Armor Not Yet Implemented");
			return;
		}

		this->SmithTreeWrapperUIIns->Render(InEquipmentType, EquipmentTree, EquipmentInfo.EquipmentId, EquipmentInfo.VariantId);
		this->SmithTreeWrapperUIIns->AddToViewport();
	}
}

void UUIManager::OnSmithTreeBack()
{
	this->CloseSmithTree(true);
}

void UUIManager::EmitSmithPicked(FName SmithResultId)
{
	if (!this->SmithTreeWrapperUIIns) return;
	this->SmithPickedDelegate.Broadcast(this->SmithTreeWrapperUIIns->EquipmentType, this->SmithTreeWrapperUIIns->ChosenEquipment, this->SmithTreeWrapperUIIns->EquipmentInsGuid, SmithResultId);
	this->CloseSmithTree(false);
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

}
