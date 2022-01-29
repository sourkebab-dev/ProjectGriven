// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "ItemDefinitions.generated.h"


UENUM(BlueprintType)
enum EItemType
{
    IT_CONSUMABLE UMETA(DisplayName = "Consumable Item"),
    IT_READABLE       UMETA(DisplayName = "Readable Item"),
    IT_GADGET UMETA(DisplayName = "Gadget Item"),
    IT_QUEST UMETA(DisplayName = "Quest Item"),
    IT_MATERIAL UMETA(DisplayName = "Material Item"),
};

UENUM(BlueprintType)
enum EItemUsageMovement
{
    IUM_BLOCK UMETA(DisplayName = "Stop Movement"),
    IUM_SLOW       UMETA(DisplayName = "Slow Down Movement"),
    IUM_NORMAL UMETA(DisplayName = "Normal Movement"),
};

UENUM(BlueprintType)
enum EItemUsageType
{
    IUT_ONEOFF UMETA(DisplayName = "One Off Item Usage"),
    IUT_TOGGLE       UMETA(DisplayName = "Toggle Item Usage"),
    IUT_HOLD UMETA(DisplayName = "Hold Item Usage"),
};

USTRUCT(BlueprintType)
struct FItemInfo
{
    GENERATED_BODY()
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ABaseItem> ItemBaseClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EItemUsageMovement> MovementType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EItemUsageType> UsageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EItemType> ItemType;

    //only for consumables
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<class UEffectPrefab*> Effects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* ItemIcon;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* UsageAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int MaxStackAmount;
    
};

USTRUCT(BlueprintType)
struct FPersistedInventoryItems
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
struct FPersistedInventory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FPersistedInventoryItems> Items;
};