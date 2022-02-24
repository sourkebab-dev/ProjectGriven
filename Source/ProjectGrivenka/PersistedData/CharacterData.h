// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/TraitSystem/TraitSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "CharacterData.generated.h"


USTRUCT(BlueprintType)
struct FPersistedCharacterAppearance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkeletalMeshId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MaterialId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> CharClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UAnimInstance> AnimClass;
};

USTRUCT(BlueprintType)
struct FPersistedCharacterBehavior
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseAIController> AIClass;
};


USTRUCT(BlueprintType)
struct FPersistedCharacterInfo
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
struct FPersistedCharacterData 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedCharacterInfo Info;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedCharacterAppearance Appearance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedTraitData Traits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedAttributes Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedEquipments Equipments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedInventory Inventory;

};