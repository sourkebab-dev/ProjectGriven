// Fill out your copyright notice in the Description page of Project Settings.


#include "UISmithTreeWrapper.h"
#include "Components/CanvasPanel.h"
#include "UISmithTree.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"

void UUISmithTreeWrapper::Render(EEquipmentType InEquipmentType, EEquipmentTree InChosenEquipment, FGuid InEquipmentInsGuid, FName InEquipmentId)
{
	this->EquipmentType = InEquipmentType;
	this->ChosenEquipment = InChosenEquipment;
	this->EquipmentInsGuid = InEquipmentInsGuid;
	this->EquipmentId = InEquipmentId;


	auto GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	auto SmithTreeClass = this->SmithTreeUIClass.FindRef(InChosenEquipment);
	auto SmithTreeUIIns = Cast<UUISmithTree>(CreateWidget(GI, SmithTreeClass, "Smith Tree UI"));
	SmithTreeUIIns->Render(InEquipmentType, InChosenEquipment, EquipmentInsGuid, EquipmentId);
	this->SmithTreePanelWrapper->AddChildToCanvas(SmithTreeUIIns);
}
