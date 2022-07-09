// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "ModularAppearanceSystemAvailable.generated.h"

UENUM(BlueprintType)
enum EBodyGroup
{
    EBG_BODYTYPE UMETA(DisplayName = "Body Types"),
    EBG_HEAD UMETA(DisplayName = "Head Size Types"),
    EBG_NOSE       UMETA(DisplayName = "Nose Types"),
    EBG_CHEEK       UMETA(DisplayName = "Cheek Types"),
    EBG_CHIN       UMETA(DisplayName = "Chin Types"),
    EBG_JAW       UMETA(DisplayName = "Jaw Types"),
    EBG_MOUTH  UMETA(DisplayName = "Mouth Types"),
    EBG_MOUTHSIZE UMETA(DisplayName = "Mouth Size"),
    EBG_MOUTHPOSITION UMETA(DisplayName = "Mouth Position"),
    EBG_HEADSIZE UMETA(DisplayName = "Head Size"),
};

USTRUCT(BlueprintType)
struct FMorphsGroupData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> MorphValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EBodyGroup> BodyGroupType;
};

USTRUCT(BlueprintType)
struct FMorphGroupNames
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> MorphIds;
};

USTRUCT(BlueprintType)
struct FBodyMorphPacks
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<TEnumAsByte<EBodyGroup>, FMorphGroupNames> GroupMorphs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> HairIds;
};

USTRUCT(BlueprintType)
struct FModularParts
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, USkeletalMeshComponent*> Partitions;
};

USTRUCT(BlueprintType)
struct FBodyInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName BodyId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FName, UTexture2D*> TextureVariants;
};

USTRUCT(BlueprintType)
struct FBodyTypeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBodyInfo Head;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBodyInfo Torso;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBodyInfo Legs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBodyInfo Hands;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBodyInfo Feet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeleton* Skeleton;
};

USTRUCT(BlueprintType)
struct FSkinColor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor MainSkinColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor SecondarySkinColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor TertiarySkinColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor OutlineSkinColor;
};


USTRUCT(BlueprintType)
struct FPersistedCharacterAppearance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName SkinColorId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName HairId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName FacialHairId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName HeadSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName MouthSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName EyeSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName LPupilSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName RPupilSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<FName, float> ShapeKeyValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<AActor> CharClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<UAnimInstance> AnimClass;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UModularAppearanceSystemAvailable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API IModularAppearanceSystemAvailable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SaveAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void LoadAppearance(FPersistedCharacterAppearance InAppearance, FPersistedEquipments InEquipments, int InGender);
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void GetModularParts(FModularParts& OutModularParts);
};
