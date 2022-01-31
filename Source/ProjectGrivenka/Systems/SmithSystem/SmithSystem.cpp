// Fill out your copyright notice in the Description page of Project Settings.


#include "SmithSystem.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

void USmithSystem::Init_Implementation() {
	Super::Init_Implementation();

	//setup ui things
}

void USmithSystem::OnReceiveSmithRequest(AActor* InSmithRequester)
{
	this->SmithRequester = InSmithRequester;
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	GI->UIManager->SmithFinishDelegate.AddDynamic(this, &USmithSystem::OnSmithFinished);
	GI->UIManager->OpenSmithUI();
}

void USmithSystem::OnSmithFinished(EEquipmentType InEquipmentType, FGuid EquipmentId, FName SmithResultId)
{
	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	GI->UIManager->SmithFinishDelegate.RemoveDynamic(this, &USmithSystem::OnSmithFinished);

	for (int i = 0; i < GI->Crew.Num(); i++) {

		if (InEquipmentType == EEquipmentType::Weapon && GI->Crew[i].Equipments.WeaponInfo.EquipmentId == EquipmentId) {
			//Sponge: needs to check if character is spawned, if true, need to reload weapon
			break;
		}

	}

	for (int i = 0; i < GI->EquipmentBox.Num(); i++) {
		if (GI->EquipmentBox[i].EquipmentAbstraction.EquipmentId == EquipmentId) {
			GI->EquipmentBox[i].EquipmentAbstraction.VariantId = SmithResultId;
			break;
		}
	}
}
