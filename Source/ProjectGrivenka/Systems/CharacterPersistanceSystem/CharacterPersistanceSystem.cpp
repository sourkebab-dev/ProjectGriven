// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPersistanceSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/TraitSystem/TraitSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

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

	this->CompContext->InfoModule.InstanceGuid = InCharacterData.Info.CharacterId;
	this->CompContext->InfoModule.InstanceName = InCharacterData.Info.CharacterName;


	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	if (!AssetsData) return;

	//Appearance Load
	FCommonSkeletalMesh* SKMeshStruct = AssetsData->SKMeshData->FindRow<FCommonSkeletalMesh>(InCharacterData.Appearance.SkeletalMeshId, "");
	FCommonMaterial* MaterialStruct = AssetsData->MaterialData->FindRow<FCommonMaterial>(InCharacterData.Appearance.MaterialId, "");
	if (!this->CompContext->SkeletalMeshComp) { GLog->Log("NOMESH"); return; }
	if (!SKMeshStruct) { GLog->Log("nomesk found"); return; }

	this->CompContext->SkeletalMeshComp->SetSkeletalMesh(SKMeshStruct->Mesh);
	this->CompContext->SkeletalMeshComp->SetMaterial(0, MaterialStruct->Material.MaterialInterface);

	if (InCharacterData.Appearance.AnimClass) {
		this->CompContext->SkeletalMeshComp->SetAnimInstanceClass(InCharacterData.Appearance.AnimClass);
	}


	//AI Load
	// sponge: might need to rethink about possession
	//this->SpawnDefaultController();

}

void UCharacterPersistanceSystem::SaveData(FPersistedCharacterData& OutCharacterData)
{
	//sponge: should implement later
}
