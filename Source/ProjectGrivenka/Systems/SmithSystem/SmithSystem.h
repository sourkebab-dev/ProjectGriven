// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "SmithSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API USmithSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	AActor* SmithRequester;

public:
	void Init_Implementation() override;

	UFUNCTION()
	void OnReceiveSmithRequest(AActor* InSmithRequester);

	UFUNCTION()
	void OnSmithFinished(EEquipmentType InEquipmentType, FGuid EquipmentId, FName SmithResultId);
};
