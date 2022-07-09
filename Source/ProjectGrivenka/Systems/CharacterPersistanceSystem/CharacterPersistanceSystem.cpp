// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPersistanceSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/TraitSystem/TraitSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/Systems/ModularAppearanceSystem/ModularAppearanceSystemAvailable.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

void UCharacterPersistanceSystem::Init_Implementation() {
	Super::Init_Implementation();

	if (!this->IsGenerated) return;
	 this->GenerateCharacter();
}

void UCharacterPersistanceSystem::GenerateWearables()
{
	FEquipmentPacks EquipmentPacks = this->GetEquipmentPacks();
	FPersistedEquipmentItem TempItem;
	if (EquipmentPacks.OuterTorsoIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.OuterTorsoIds);
		this->CharacterInitializer.Equipments.OuterTorsoWearable = TempItem;
	}

	if (EquipmentPacks.InnerTorsoIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.InnerTorsoIds);
		this->CharacterInitializer.Equipments.TorsoWearable = TempItem;
	}

	if (EquipmentPacks.PantsIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.PantsIds);
		this->CharacterInitializer.Equipments.LegsWearable = TempItem;
	}

	if (EquipmentPacks.GlovesIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.GlovesIds);
		this->CharacterInitializer.Equipments.HandsWearable = TempItem;
	}

	if (EquipmentPacks.ShoesIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.ShoesIds);
		this->CharacterInitializer.Equipments.FootWearable = TempItem;
	}

	if (EquipmentPacks.HeadWearableIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.HeadWearableIds);
		this->CharacterInitializer.Equipments.HeadWearable = TempItem;
	}

	if (EquipmentPacks.AccHeadIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.AccHeadIds);
		this->CharacterInitializer.Equipments.Accessories1Wearable = TempItem;
	}

	if (EquipmentPacks.AccBodyIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.AccBodyIds);
		this->CharacterInitializer.Equipments.Accessories2Wearable = TempItem;
	}

	if (EquipmentPacks.AccLegsIds.Num()) {
		TempItem.VariantId = this->GetRandomEquipmentVariants(EquipmentPacks.AccLegsIds);
		this->CharacterInitializer.Equipments.Accessories3Wearable = TempItem;
	}
}

void UCharacterPersistanceSystem::GenerateBody()
{
	auto MorphPacks = this->GetBodyMorphPacks();
	TArray<TEnumAsByte<EBodyGroup>> BodyGroups;
	MorphPacks.GroupMorphs.GetKeys(BodyGroups);
	UGrivenkaDataSingleton* GrivenkaSingleton = UGrivenkaSingletonLibrary::GetGrivenkaData();

	for (int i = 0; i < BodyGroups.Num(); i++) {
		TArray<FName> MorphIds = MorphPacks.GroupMorphs.FindRef(BodyGroups[i]).MorphIds;
		int MaxNum = MorphIds.Num();
		int RandIdx = FMath::RandRange(0, MaxNum - 1);
		FName RandId = MorphIds[RandIdx];

		this->CharacterInitializer.Appearance.ShapeKeyValues.Append(GrivenkaSingleton->GetMorphGroupValues(RandId));
	}

	int RandHairIdx = FMath::RandRange(0, MorphPacks.HairIds.Num() - 1);
	FName HairId = MorphPacks.HairIds[RandHairIdx];
	this->CharacterInitializer.Appearance.HairId = HairId;
}

void UCharacterPersistanceSystem::GenerateCharacter()
{
	this->GenerateWearables();
	this->GenerateBody();

	this->LoadData(this->CharacterInitializer);
}

FName UCharacterPersistanceSystem::GetRandomEquipmentVariants(TArray<FEquipmentChance> InEquipmentPack)
{

	float chance = FMath::RandRange(0, 100);
	float sum = 0.0f;

	for (int i = 0; i < InEquipmentPack.Num(); i++) {
		sum += InEquipmentPack[i].Chance;
		if (chance <= sum) {
			return InEquipmentPack[i].EquipmentId;
		}
	}

	return "";
}

void UCharacterPersistanceSystem::LoadData(FPersistedCharacterData InCharacterData)
{
	if (this->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_LoadAttributes(this->CompContext->CharacterActor, InCharacterData.Attributes);
	}

	if (this->CompContext->CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		IEquipmentSystemAvailable::Execute_LoadEquipments(this->CompContext->CharacterActor, InCharacterData.Equipments);
	}

	if (this->CompContext->CharacterActor->Implements<UCharacterInventoryAvailable>()) {
		ICharacterInventoryAvailable::Execute_LoadInventory(this->CompContext->CharacterActor, InCharacterData.Inventory);
	}

	if (this->CompContext->CharacterActor->Implements<UTraitSystemAvailable>()) {
		ITraitSystemAvailable::Execute_LoadTraits(this->CompContext->CharacterActor, InCharacterData.Traits);
	}

	if (this->CompContext->CharacterActor->Implements<UModularAppearanceSystemAvailable>()) {
		IModularAppearanceSystemAvailable::Execute_LoadAppearance(this->CompContext->CharacterActor, InCharacterData.Appearance, InCharacterData.Equipments, InCharacterData.Info.Gender);
	}

	if (InCharacterData.Appearance.AnimClass) {
		this->CompContext->SkeletalMeshComp->SetAnimInstanceClass(InCharacterData.Appearance.AnimClass);
	}

	this->CompContext->InfoModule.InstanceGuid = InCharacterData.Info.CharacterId;
	this->CompContext->InfoModule.InstanceName = InCharacterData.Info.CharacterName;

	//AI Load
	// sponge: might need to rethink about possession
	//this->SpawnDefaultController();

}

void UCharacterPersistanceSystem::SaveData(FPersistedCharacterData& OutCharacterData)
{
	//sponge: should implement later
}
