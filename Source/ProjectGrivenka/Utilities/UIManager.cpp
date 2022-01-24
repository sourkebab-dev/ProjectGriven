// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManager.h"
#include "Blueprint/UserWidget.h"
#include "BaseGameInstance.h"
#include "ProjectGrivenka/UI/Dialogue/DialogueBox.h"
#include "ProjectGrivenka/UI/ItemBelt/ActiveItemBelt.h"
#include "ProjectGrivenka/UI/Loot/UILootListNotify.h"

void UUIManager::Init(UBaseGameInstance* InGameIns) {
	this->GameIns = InGameIns;
}

void UUIManager::LoadInGameUI()
{
	this->ActiveItemBeltUIIns = Cast<UActiveItemBelt>(CreateWidget(this->GameIns, this->ActiveItemBeltUIClass, "Active Item Belt UI"));
	this->ActiveItemBeltUIIns->AddToViewport();
}

void UUIManager::SetActiveItemImage(FItemInfo InItemInfo)
{
	this->ActiveItemBeltUIIns->SetItemInfo(InItemInfo);
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

void UUIManager::EmitChangeEquipment(FPersistedEquipmentItem EquipmentInfo)
{
	if (!this->GameIns) return;
	this->EquipmentChangeDelegate.Broadcast(this->GameIns->GetControlledCrewId(), EquipmentInfo);
}

void UUIManager::EmitProceedDialogue()
{
	this->DialogueProceedDelegate.Broadcast();
}

void UUIManager::EmitReplyDialogue(FName InReplyId)
{
	this->DialogueReplyDelegate.Broadcast(InReplyId);
}