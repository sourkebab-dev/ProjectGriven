// Fill out your copyright notice in the Description page of Project Settings.


#include "UIEquipmentItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "ProjectGrivenka/Utilities/UIManager.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Equipments/BaseEquipment.h"


void UUIEquipmentItem::NativeConstruct()
{
	Super::NativeConstruct();
	//Note: aDD DYNAMIC NEEDS to be removed on endplay/uidestruct
	this->SlotButton->OnClicked.AddDynamic(this, &UUIEquipmentItem::OnSlotClick);
}

void UUIEquipmentItem::NativeDestruct()
{
	this->SlotButton->OnClicked.RemoveDynamic(this, &UUIEquipmentItem::OnSlotClick);
	Super::NativeDestruct();
}

void UUIEquipmentItem::SetSlotInfo(FEquipmentBoxItem ItemInfo)
{
	if (!ItemInfo.EquipmentAbstraction.BaseClass) return;
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UTexture2D* SlotIcon;
	if (!ItemInfo.EquipmentAbstraction.VariantId.IsNone() && AssetsData) {
		UWeaponPrefabs* WeaponPrefab = AssetsData->WeaponPrefabs->WeaponAssets.FindRef(ItemInfo.EquipmentAbstraction.VariantId);
		if (!WeaponPrefab) return;
		FWeaponInfo WeapInfo = WeaponPrefab->WeaponInfo;
		SlotIcon = WeapInfo.GeneralInfo.EquipmentIcon;
		GLog->Log(SlotIcon->GetFullName());
	}
	else {
		ABaseEquipment* Equipment = ItemInfo.EquipmentAbstraction.BaseClass.GetDefaultObject();
		SlotIcon = Equipment->EquipmentIcon;
		GLog->Log("B");
	}

	if (SlotIcon) this->SlotImage->SetBrushFromTexture(SlotIcon, true);
	this->SlotInfo = ItemInfo;
	GLog->Log("Info Set");
}

void UUIEquipmentItem::OnSlotClick()
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance || !GameInstance->UIManager) return;

	GameInstance->UIManager->EmitChangeEquipment(this->SlotInfo.EquipmentAbstraction);
	GLog->Log("OnSlotClick");
}
