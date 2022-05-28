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
	GI->UIManager->UICloseDelegate.AddDynamic(this, & USmithSystem::DisconnectUI);
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 2, FColor::Yellow, "Smithy open");
	GI->UIManager->OpenSmithEQBoxUI();
}

void USmithSystem::DisconnectUI()
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	GI->UIManager->SmithPickedDelegate.RemoveDynamic(this, &USmithSystem::OnSmithFinished);
	GI->UIManager->UICloseDelegate.RemoveDynamic(this, &USmithSystem::DisconnectUI);
}

void USmithSystem::OnSmithFinished(EEquipmentType InEquipmentType, EEquipmentTree InChosenEquipment, FGuid InstanceGuid, FName SmithResultId)
{
	this->DisconnectUI();
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	
	TArray<FEquipmentBoxItem>* SelectedEqBox = nullptr;
	if (InEquipmentType == EEquipmentType::Weapon) {
		SelectedEqBox = &GI->WeaponBox;

	}
	else {
		SelectedEqBox = &GI->ArmorBox;
	}

	for (int i = 0; i < SelectedEqBox->Num(); i++) {
		if ((*SelectedEqBox)[i].EquipmentAbstraction.EquipmentId == InstanceGuid) {
			(*SelectedEqBox)[i].EquipmentAbstraction.VariantId = SmithResultId;
			break;
		}
	}

	int CrewIndex = -1;
	for (int i = 0; i < GI->Crew.Num(); i++) {
		if (InEquipmentType == EEquipmentType::Weapon && GI->Crew[i].Equipments.WeaponInfo.EquipmentId == InstanceGuid) {
			CrewIndex = i;
			break;
		}
	}

	if (CrewIndex < 0) return;

	AActor* UpgradedCharInstance = nullptr;

	auto PlayerCtx = IContextAvailable::Execute_GetContext(this->GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerCtx->InfoModule.InstanceGuid == GI->Crew[CrewIndex].Info.CharacterId) {
		UpgradedCharInstance = this->GetWorld()->GetFirstPlayerController()->GetPawn();
	}
	else {
		for (int i = 0; i < GI->PartyInstance.Num(); i++) {
			auto Ctx = IContextAvailable::Execute_GetContext(GI->PartyInstance[i]);
			if (Ctx->InfoModule.InstanceGuid == GI->Crew[CrewIndex].Info.CharacterId) {
				UpgradedCharInstance = GI->PartyInstance[i];
			}
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
		GEngine->AddOnScreenDebugMessage(FMath::Rand(), 2, FColor::Yellow, "Loaded");
	}
}
