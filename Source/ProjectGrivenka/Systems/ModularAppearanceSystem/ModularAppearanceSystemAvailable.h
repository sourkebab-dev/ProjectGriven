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
    
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Hair;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* HeadWear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* FacialHair;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* OuterTorso;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Torso;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Legs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Hands;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Feet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Acc1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Acc2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMeshComponent* Acc3;

};

USTRUCT(BlueprintType)
struct FBodyInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName BodyId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        USkeletalMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UTexture2D* TextureSlots;

};


USTRUCT(BlueprintType)
struct FPersistedCharacterAppearance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName HairId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName FacialHairId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName BodySkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName HeadSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName MouthSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName EyeSkinId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FName PupilSkinId;

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
