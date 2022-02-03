// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "EquipmentDefinitions.generated.h"


UENUM(BlueprintType)
enum EEquipmentTree
{
    KATANA_TREE UMETA(DisplayName = "KATANA TREE"),
    HAMMER_TREE       UMETA(DisplayName = "HAMMER TREE"),
};


UENUM(BlueprintType)
enum EEquipmentType
{
    Weapon UMETA(DisplayName = "Weapon Equipment"),
    Armor       UMETA(DisplayName = "Armor Equipment"),
};

USTRUCT(BlueprintType)
struct FPersistedEquipmentItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGuid EquipmentId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName VariantId;
};

USTRUCT(BlueprintType)
struct FPersistedEquipments
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FPersistedEquipmentItem WeaponInfo;
};

USTRUCT(BlueprintType)
struct FBlockInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* BlockMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* BlockPushMontage;

};

USTRUCT(BlueprintType)
struct FEquipmentInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ABaseEquipment> EquipmentBaseClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EEquipmentTree> EquipmentTree;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EEquipmentType> EquipmentType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EquipmentId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EquipmentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EquipmentDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMesh* EquipmentMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UMaterialInterface*> EquipmentMeshMaterials;

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

    //Note: Block Damage Absorption
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float DamageAbsorption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float FortitudeDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float RawStaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    float ElementalDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Values")
    TEnumAsByte<EDamageElementType> ElementType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocks")
    FBlockInfo BlockInfo;
};

USTRUCT(BlueprintType)
struct FEquipmentBoxItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Abstraction")
    FPersistedEquipmentItem EquipmentAbstraction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    int BoxIndex;
};


USTRUCT(BlueprintType)
struct FSmithingRecipeItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UItemPrefab* MaterialItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Count;
};


USTRUCT(BlueprintType)
struct FSmithingRecipeTreeItem
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USmithingRecipePrefab* Recipe;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int> ChildrenIndex;
};

