// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlot/UIEquipmentItem.h"
#include "UIEquipmentBoxContainer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUIEquipmentBoxContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUniformGridPanel* EquipmentGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ColumnNumber = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SlotNumber = 20;

public: 
	UFUNCTION(BlueprintCallable)
	void DataSetup(TSubclassOf<UUIEquipmentItem> EquipmentItemClass);
};
