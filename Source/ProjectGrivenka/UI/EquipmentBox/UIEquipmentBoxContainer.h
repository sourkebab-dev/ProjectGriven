// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlot/UIEquipmentItem.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
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
	class UButton* WeaponTabButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ArmorTabButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUniformGridPanel* EquipmentGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ColumnNumber = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SlotNumber = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEquipmentType> CurrentEqType = EEquipmentType::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUIEquipmentItem> EquipmentItemClass;

public: 
	void NativeOnInitialized() override;
	void NativeDestruct() override;

	UFUNCTION()
	void OnArmorTabClick();
	UFUNCTION()
	void OnWeaponTabClick();
	UFUNCTION(BlueprintCallable)
	void DataSetup();
};
