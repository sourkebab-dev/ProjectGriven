// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "UISmithTreeWrapper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUISmithTreeWrapper : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUISmithItemDetail* SmithTreeDetail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* SmithTreePanelWrapper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EEquipmentTree>, TSubclassOf<class UUISmithTree>> SmithTreeUIClass;

	EEquipmentType EquipmentType;
	EEquipmentTree ChosenEquipment;
	FGuid EquipmentInsGuid;
	FName EquipmentId;

public:
	UFUNCTION(BlueprintCallable)
	void Render(EEquipmentType InEquipmentType, EEquipmentTree InChosenEquipment, FGuid InEquipmentInsGuid, FName InEquipmentId);

};
