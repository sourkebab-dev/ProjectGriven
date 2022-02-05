// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Items/ItemDefinitions.h"
#include "UIItemPouchSlot.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUIItemPouchSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* SlotImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ItemCount;

	UPROPERTY(BlueprintReadWrite)
	FPersistedInventoryItems ItemInfo;

	UPROPERTY(BlueprintReadWrite)
	AActor* Owner;

public:
	UFUNCTION(BlueprintCallable)
	void Render(AActor* InOwner, FPersistedInventoryItems InInventoryItemInfo);
};
