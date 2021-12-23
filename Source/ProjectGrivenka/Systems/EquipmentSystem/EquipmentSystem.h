// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "EquipmentSystem.generated.h"


class ABaseWeapon;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UEquipmentSystem : public UBaseContextableComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABaseWeapon* WeaponR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABaseWeapon* WeaponL;


public:
	void Init() override;

	UFUNCTION(BlueprintCallable)
	void DisableWeaponDamageColliders();

	UFUNCTION(BlueprintCallable)
	virtual void LoadEquipments(FPersisted_CharacterCompleteData CharacterData);

	UFUNCTION(BlueprintCallable)
	virtual ABaseWeapon* GetEquippedWeapon() { return WeaponR; };
	
	UFUNCTION(BlueprintCallable)
	virtual void SetEquippedWeapon(ABaseWeapon* InWeapon) { WeaponR = InWeapon;  };

	UFUNCTION(BlueprintCallable)
	virtual float CalculateAttackStaminaCost(FAttackValues InCurrentAttack);
	
	UFUNCTION(BlueprintCallable)
	virtual FAttackValues GetNextAttackValue(TEnumAsByte<enum EAttackMovementType> InMovementType, FAttackValues InCurrentAttack);
};