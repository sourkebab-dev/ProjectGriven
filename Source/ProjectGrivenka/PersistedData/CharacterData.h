// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterData.generated.h"


USTRUCT(BlueprintType)
struct FPersisted_CharacterAppearance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkeletalMeshId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MaterialId;

	//SPONGE: something is not allowing me to save a subclass of ABaseCharacter (undeclared identifier)
	//had to forward declare i dunno why 
	//Note: turns out it's a circular dependency issue
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseCharacter> CharClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UAnimInstance> AnimClass;
};

USTRUCT(BlueprintType)
struct FPersisted_CharacterAttributes
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
		float MaxAmp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
		float Amp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
		float AmpRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float StaminaRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
		float MaxFortitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
		float Fortitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
		float FortitudeRecoverRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
		float Defense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float WeaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float WeaponMovingValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float ElemFireDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float ElemIceDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float ElemElectricDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float ElemPoisonDefense;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		float ElemEnergyDefense;
};

USTRUCT(BlueprintType)
struct FPersisted_EquipmentInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid EquipmentId;

	//Note: Base Class members will be overriden by variant info
	//SPonge: maybe put baseclass inside variant? also should probably add an equipment type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseEquipment> BaseClass;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VariantId;
};


USTRUCT(BlueprintType)
struct FPersisted_CharacterEquipments
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_EquipmentInfo WeaponInfo;
};

USTRUCT(BlueprintType)
struct FPersisted_CharacterItems
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemId;

};

USTRUCT(BlueprintType)
struct FPersisted_CharacterBehavior
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseAIController> AIClass;
};

USTRUCT(BlueprintType)
struct FPersisted_CharacterInventory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPersisted_CharacterItems> Items;
};

USTRUCT(BlueprintType)
struct FPersisted_CharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid CharacterId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CharacterAge;
};


USTRUCT(BlueprintType)
struct FPersisted_CharacterCompleteData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_CharacterInfo Info;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_CharacterAppearance Appearance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_CharacterAttributes Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_CharacterEquipments Equipments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_CharacterInventory Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersisted_CharacterBehavior Behavior;
};