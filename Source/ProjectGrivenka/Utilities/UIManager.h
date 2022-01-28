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

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UUIManager : public UObject
{
	GENERATED_BODY()


public:
	//SPONGE: maybe theres a better way to do this
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UBaseGameInstance* GameIns;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UUILootListNotify> LootListUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UDialogueBox> DialogueBoxUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main UI Classes")
	TSubclassOf<class UActiveItemBelt> ActiveItemBeltUIClass;
	
public:

	void Init(class UBaseGameInstance* InGameIns);

	UFUNCTION(BlueprintCallable)
	void LoadInGameUI();

	UFUNCTION(BlueprintCallable)
	void SetActiveItemImage(FItemInfo InItemInfo);

	UFUNCTION(BlueprintCallable)
	void ShowDialogue(FDialogueData InDialogueData);

	UFUNCTION(BlueprintCallable)
	void ShowDialogueReply(TArray<FReplyData> InReplyData, FDialogueData InDialogueData);

	UFUNCTION(BlueprintCallable)
	void OpenDialogueBox();

	UFUNCTION(BlueprintCallable)
	void CloseDialogueBox();

	UFUNCTION(BlueprintCallable)
	void EnqueueLootDisplay(FLootItem InLootItem);

	UFUNCTION(BlueprintCallable)
	void RemoveLootDisplay();

	UFUNCTION(BlueprintCallable)
	void EmitChangeEquipment(FPersistedEquipmentItem EquipmentInfo);

	UFUNCTION(BlueprintCallable)
	void EmitProceedDialogue();

	UFUNCTION(BlueprintCallable)
	void EmitReplyDialogue(FName InReplyId);

};
