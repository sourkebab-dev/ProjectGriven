// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "EquipmentDefinitions.generated.h"


USTRUCT(BlueprintType)
struct FEquipmentInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EquipmentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EquipmentDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMesh* EquipmentMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* EquipmentIcon;

};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Info")
    FEquipmentInfo GeneralInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float RawDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float FortitudeDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float RawStaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float ElementalDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    TEnumAsByte<EDamageElementType> ElementType;
};

USTRUCT(BlueprintType)
struct FEquipmentBoxItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Abstraction")
    FPersisted_EquipmentInfo EquipmentAbstraction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    int BoxIndex;
};

UENUM(BlueprintType)
enum EEquipmentType
{
    Weapon UMETA(DisplayName = "Weapon Equipment"),
    Armor       UMETA(DisplayName = "Armor Equipment"),
};