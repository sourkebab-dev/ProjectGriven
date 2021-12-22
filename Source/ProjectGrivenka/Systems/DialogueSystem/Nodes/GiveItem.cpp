// Fill out your copyright notice in the Description page of Project Settings.


#include "GiveItem.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventory.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/Items/ItemPrefab.h"

UGiveItem::UGiveItem() {
	this->NodeName = TEXT("Give Item");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UGiveItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!this->GetWorld() || !this->GetWorld()->GetFirstPlayerController() || !this->GetWorld()->GetFirstPlayerController()->GetCharacter()) return EBTNodeResult::Failed;
	AActor* InventoryActor = Cast<AActor>(this->GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (!InventoryActor->Implements<UCharacterInventoryAvailable>()) return EBTNodeResult::Failed;
	for (int i = 0; i < this->ItemData.Num(); i++) {
		ICharacterInventoryAvailable::Execute_GetCharacterInventoryComp(InventoryActor)->AddItemToInventory(this->ItemData[i].GivenItem->ItemInfo.ItemId, this->ItemData[i].ItemCount);
	}
	return EBTNodeResult::Succeeded;
}
