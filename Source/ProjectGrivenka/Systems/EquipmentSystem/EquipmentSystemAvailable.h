// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "EquipmentSystemAvailable.generated.h"




// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEquipmentSystemAvailable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API IEquipmentSystemAvailable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UEquipmentSystem* GetEquipmentSystemComp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SaveEquipments(FPersistedEquipments InEquipments);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LoadEquipments(FPersistedEquipments InEquipments);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisableWeaponDamageColliders();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetNextMainAttack(enum EAttackMovementType InMovementType, FAttackValues InCurrentAttackValues, FAttackValues& OutNextAttack);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetCurrentAttackStaminaUsage(FAttackValues InCurrentAttackValues);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetBlockInfo(FBlockInfo& OutBlockInfo, float& OutDamageAbsorption);
};
