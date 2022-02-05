// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterInventoryAvailable.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/Items/ItemDefinitions.h"
#include "CharacterInventory.generated.h"

//Sponge: Need to handle, inventory stacking, change from inventory to itembelt, item use till 0 handle in itembelt, inventory available slot optimizations
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UCharacterInventory : public UBaseContextableComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedInventory InventoryList;

	//int points to index in inventorylist
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> ItemBelt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SelectedItemBeltIdx = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CommitedItemBeltIdx = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABaseItem* UsedItem;

	bool IsUIShown = false;

public:	
	void Init_Implementation() override;
	UFUNCTION(BlueprintCallable)
	void SaveData();
	UFUNCTION(BlueprintCallable)
	void LoadData(FPersistedInventory InInventory);
	UFUNCTION(BlueprintCallable)
	virtual void SetSelectedItem(int Idx);
	UFUNCTION(BlueprintCallable)
	virtual class UItemPrefab* GetSelectedItemInventory();
	UFUNCTION(BlueprintCallable)
	virtual void UseSelectedItemBelt();
	UFUNCTION(BlueprintCallable)
	virtual void CommitItem();
	UFUNCTION(BlueprintCallable)
	virtual void AddItemToInventory(FName InItemId, int InItemCount);
	UFUNCTION(BlueprintCallable)
	virtual void AddToStack(int InInventoryIndex, int InToFill);
	UFUNCTION(BlueprintCallable)
	virtual void SyncItemBeltUI();
	UFUNCTION(BlueprintCallable)
	virtual void ToggleShowInventory();
	UFUNCTION(BlueprintCallable)
	virtual void MoveSlot(FName ItemId, int SourceIndex, int NewSourceIndex);
};
