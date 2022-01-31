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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipmentChangeDelegate, FGuid, CharacterId, FPersistedEquipmentItem, EquipmentInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueProceedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDialogueReplyDelegate, FName, ReplyId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSmithFinishedDelegate, EEquipmentType, InEquipmentType, FGuid, EquipmentId, FName, SmithResultId);

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UUIManager : public UObject
{
	GENERATED_BODY()


public:
	//SPONGE: maybe theres a better way to do this
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UBaseGameInstance* GameIns;

	UPROPERTY(BlueprintAssignable)
	FSmithFinishedDelegate SmithFinishDelegate;

	UPROPERTY(BlueprintAssignable)
	FEquipmentChangeDelegate EquipmentChangeDelegate;

	UPROPERTY(BlueprintAssignable)
	FDialogueProceedDelegate DialogueProceedDelegate;

	UPROPERTY(BlueprintAssignable)
	FDialogueReplyDelegate DialogueReplyDelegate;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UDialogueBox* DialogueBoxUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UActiveItemBelt* ActiveItemBeltUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UUILootListNotify* LootListUIIns;

	UPROPERTY(BlueprintReadWrite, Category = "Main UI Instance")
	class UPlayerStatsContainer* PlayerStatsContainerUIIns;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UUILootListNotify> LootListUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UDialogueBox> DialogueBoxUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UActiveItemBelt> ActiveItemBeltUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UPlayerStatsContainer> PlayerStatsContainerUIClass;

public:

	void Init(class UBaseGameInstance* InGameIns);

	UFUNCTION(BlueprintCallable)
	void StatsSetup();

	UFUNCTION(BlueprintCallable)
	void LoadInGameUI();

	UFUNCTION(BlueprintCallable)
	void SetActiveItemImage(FItemInfo InItemInfo);

#pragma region Equipment Box
	UFUNCTION(BlueprintCallable)
	void EmitChangeEquipment(FPersistedEquipmentItem EquipmentInfo);
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
	void OpenSmithUI();

	UFUNCTION(BlueprintCallable)
	void EmitSmithFinished(EEquipmentType InEquipmentType, FGuid EquipmentId, FName SmithResultId);
#pragma endregion
};
