// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Systems/LootSystem/LootSystemDefinitions.h"
#include "UILootListNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUILootListNotify : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* LootIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LootName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LootNumber;

	TQueue<FLootItem> LootQueue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle QueueTimer;

public:
	UFUNCTION(BlueprintCallable)
	void DequeueLootDisplay();

	UFUNCTION(BlueprintCallable)
	void EnqueueLootDisplay(FLootItem InLootItem);

	UFUNCTION(BlueprintCallable)
	void SetItemInfo(FLootItem InLootItem);
};
