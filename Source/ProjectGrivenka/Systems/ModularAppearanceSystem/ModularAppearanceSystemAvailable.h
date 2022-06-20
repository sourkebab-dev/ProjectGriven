// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "ModularAppearanceSystemAvailable.generated.h"

USTRUCT(BlueprintType)
struct FModularParts
{
    GENERATED_BODY()

    // Is used to transfer a baked normal to the morphed ones
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMeshComponent* TempBakedMesh;

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
