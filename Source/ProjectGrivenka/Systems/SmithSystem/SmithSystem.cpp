// Fill out your copyright notice in the Description page of Project Settings.


#include "SmithSystem.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"

void USmithSystem::OnReceiveSmithRequest()
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	GI->UIManager->SmithPickedDelegate.AddDynamic(this, &USmithSystem::OnSmithFinished);
	GI->UIManager->OpenSmithUI();
}

void USmithSystem::OnSmithFinished(EEquipmentType InEquipmentType, EEquipmentTree InChosenEquipment, FGuid InstanceGuid, FName SmithResultId)
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	GI->UIManager->SmithPickedDelegate.RemoveDynamic(this, &USmithSystem::OnSmithFinished);
	
	TArray<FEquipmentBoxItem> SelectedEqBox;
	if (InEquipmentType == EEquipmentType::Armor) {
		SelectedEqBox = GI->ArmorBox;
	}
	else if (InEquipmentType == EEquipmentType::Weapon) {
		SelectedEqBox = GI->WeaponBox;
	}


	for (int i = 0; i < SelectedEqBox.Num(); i++) {
		if (SelectedEqBox[i].EquipmentAbstraction.EquipmentId == InstanceGuid) {
			SelectedEqBox[i].EquipmentAbstraction.VariantId = SmithResultId;
			break;
		}
	}

	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 2, FColor::Yellow, "Sheeeshhh1");
	int CrewIndex = -1;
	for (int i = 0; i < GI->Crew.Num(); i++) {
		if (InEquipmentType == EEquipmentType::Weapon && GI->Crew[i].Equipments.WeaponInfo.EquipmentId == InstanceGuid) {
			CrewIndex = i;
			break;
		}
	}

	if (CrewIndex > 0) {
		AActor* UpgradedCharInstance = nullptr;
		for (int i = 0; i < GI->PartyInstance.Num(); i++) {
			auto Ctx = IContextAvailable::Execute_GetContext(GI->PartyInstance[i]);
			if (Ctx->InfoModule.InstanceGuid == GI->Crew[CrewIndex].Info.CharacterId) {
				UpgradedCharInstance = GI->PartyInstance[i];
			}
		}

		if (UpgradedCharInstance && UpgradedCharInstance->Implements<UEquipmentSystemAvailable>()) {
			FPersistedEquipments EquipmentInfo;
			if (InEquipmentType == EEquipmentType::Weapon) {
				GI->Crew[CrewIndex].Equipments.WeaponInfo.VariantId = SmithResultId;
				EquipmentInfo.WeaponInfo.EquipmentId = InstanceGuid;
				EquipmentInfo.WeaponInfo.VariantId = SmithResultId;
			}
			IEquipmentSystemAvailable::Execute_LoadEquipments(UpgradedCharInstance, EquipmentInfo);
		}
	}

}
