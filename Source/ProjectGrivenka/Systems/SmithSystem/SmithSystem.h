// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "SmithSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIVENKA_API USmithSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void OnReceiveSmithRequest();

	UFUNCTION(BlueprintCallable)
	void DisconnectUI();

	UFUNCTION(BlueprintCallable)
	void OnSmithFinished(EEquipmentType InEquipmentType, EEquipmentTree InChosenEquipment, FGuid InstanceGuid, FName SmithResultId);
};
