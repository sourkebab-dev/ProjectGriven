// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/Items/ItemDefinitions.h"
#include "ProjectGrivenka/Systems/LootSystem/LootSystemDefinitions.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "UIManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipmentBoxClicked, FPersistedEquipmentItem, EquipmentInfo, EEquipmentType, EquipmentType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUISimpleDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueReplyDelegate, FName, ReplyId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSmithPickedDelegate, EEquipmentType, InEquipmentType, EEquipmentTree, InChosenEquipment, FGuid, EquipmentId, FName, SmithResultId);

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UUIManager : public UObject
{
	GENERATED_BODY()


public:
	//SPONGE: maybe theres a better way to do this
	UPROPERTY(BlueprintAssignable)
	FUISimpleDelegate UIBackDelegate;

	UPROPERTY(BlueprintAssignable)
	FUISimpleDelegate UICloseDelegate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UBaseGameInstance* GameIns;

	UPROPERTY(BlueprintAssignable)
	FSmithPickedDelegate SmithPickedDelegate;

	UPROPERTY(BlueprintAssignable)
	FEquipmentBoxClicked EquipmentBoxClickedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUISimpleDelegate DialogueProceedDelegate;

	UPROPERTY(BlueprintAssignable)
	FDialogueReplyDelegate DialogueReplyDelegate;

	//Sponge: might need to remove these instances

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UDialogueBox* DialogueBoxUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UActiveItemBelt* ActiveItemBeltUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UUILootListNotify* LootListUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UPlayerStatsContainer* PlayerStatsContainerUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UUIEquipmentBoxContainer* EquipmentBoxUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UUISmithTreeWrapper* SmithTreeWrapperUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	TMap<AActor*, class UUIInventoryContainer*> InventoryUIIns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UUILootListNotify> LootListUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UDialogueBox> DialogueBoxUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UActiveItemBelt> ActiveItemBeltUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UPlayerStatsContainer> PlayerStatsContainerUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UUISmithTreeWrapper> SmithTreeWrapperUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UUIEquipmentBoxContainer> EquipmentBoxUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UUIInventoryContainer> InventoryUIClass;

public:

	void Init(class UBaseGameInstance* InGameIns);

	UFUNCTION(BlueprintCallable)
	void StatsSetup();

	UFUNCTION(BlueprintCallable)
	void LoadInGameUI();


	UFUNCTION(BlueprintCallable)
	void EmitUIClose();

	UFUNCTION(BlueprintCallable)
	void EmitUIBack();

#pragma region Inventory
	UFUNCTION(BlueprintCallable)
	void OpenInventoryUI(AActor* Owner);

	UFUNCTION(BlueprintCallable)
	void CloseInventoryUI(AActor* Owner);

	UFUNCTION(BlueprintCallable)
	void SetActiveItemImage(FItemInfo InItemInfo);
#pragma endregion

#pragma region Equipment Box
	UFUNCTION(BlueprintCallable)
	void EmitEquipmentBoxClicked(FPersistedEquipmentItem EquipmentInfo, EEquipmentType InEquipmentType);
#pragma endregion

#pragma region Loot System
	UFUNCTION(BlueprintCallable)
	void EnqueueLootDisplay(FLootItem InLootItem);

	UFUNCTION(BlueprintCallable)
	void RemoveLootDisplay();
#pragma endregion

#pragma region Dialogue System
	UFUNCTION(BlueprintCallable)
	void ShowDialogue(FDialogueData InDialogueData);

	UFUNCTION(BlueprintCallable)
	void ShowDialogueReply(TArray<FReplyData> InReplyData, FDialogueData InDialogueData);

	UFUNCTION(BlueprintCallable)
	void OpenDialogueBox();

	UFUNCTION(BlueprintCallable)
	void CloseDialogueBox();

	UFUNCTION(BlueprintCallable)
	void EmitProceedDialogue();

	UFUNCTION(BlueprintCallable)
	void EmitReplyDialogue(FName InReplyId);
#pragma endregion

#pragma region Smith System
	UFUNCTION(BlueprintCallable)
	void OpenSmithEQBoxUI();

	UFUNCTION(BlueprintCallable)
	void CloseSmithEQBoxUI();

	UFUNCTION(BlueprintCallable)
	void OnSmithEQBoxChosen(FPersistedEquipmentItem EquipmentInfo, EEquipmentType InEquipmentType);

	UFUNCTION(BlueprintCallable)
	void CloseSmithTree(bool IsBack);
	
	UFUNCTION(BlueprintCallable)
	void OnSmithTreeBack();

	UFUNCTION(BlueprintCallable)
	void EmitSmithPicked(FName SmithResultId);
#pragma endregion
};
