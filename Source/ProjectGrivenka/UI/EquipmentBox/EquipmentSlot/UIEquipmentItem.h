// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "UIEquipmentItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUIEquipmentItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* SlotButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* SlotOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* SlotImage;

	FEquipmentBoxItem SlotInfo;
public:
	UFUNCTION(BlueprintCallable)
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void SetSlotInfo(FEquipmentBoxItem ItemInfo);

	UFUNCTION(BlueprintCallable)
	void OnSlotClick();
};
