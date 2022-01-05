// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GlobalDefinitions.generated.h"

UENUM(BlueprintType)
enum EAttackMovementType {
    AM_DEFAULT       UMETA(DisplayName = "Default Attack Movement"),
    AM_HEAVY       UMETA(DisplayName = "Heavy Attack Movement"),
    AM_CHARGED       UMETA(DisplayName = "Charged Attack Movement"),
    AM_LUNGE       UMETA(DisplayName = "Lunge Attack Movement"),
};

UENUM(BlueprintType)
enum EDamageImpactType {
    DI_LOW       UMETA(DisplayName = "Default Attack Movement"),
    DI_MEDIUM       UMETA(DisplayName = "Heavy Attack Movement"),
    DI_HIGH       UMETA(DisplayName = "Charged Attack Movement"),
};

USTRUCT(BlueprintType)
struct FAttackValues
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int NextComboPointer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int NextHeavyPointer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovingValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EDamageImpactType> ImpactType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector AttackDirection;
};

USTRUCT(BlueprintType)
struct FRotationRate
{
    GENERATED_BODY()
    
    FRotationRate() : OffRotationRate(0), SlowRotationRate(0), NormalRotationRate(0) {}
    FRotationRate(const float NewOffRotationRate, const float NewSlowRotationRate, const float NewNormalRotationRate ) : OffRotationRate(NewOffRotationRate), SlowRotationRate(NewSlowRotationRate), NormalRotationRate(NewNormalRotationRate) {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
     float OffRotationRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
     float SlowRotationRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
     float NormalRotationRate;
};

USTRUCT(BlueprintType)
struct FCommonCharacterAnimation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* LKnockLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* LKnockRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* LKnockForward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* LKnockBack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* MKnockForward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* MKnockBack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* HKnockForward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* HKnockBack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ToggleAmpField;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* VentAmp;

};

UENUM(BlueprintType)
enum EDamageElementType
{
    ElemNone UMETA(DisplayName= "No Element Type"),
    ElemFire       UMETA(DisplayName = "Fire Element Type"),
    ElemPoison       UMETA(DisplayName = "Poison Element Type"),
    ElemIce        UMETA(DisplayName = "Ice Element Type"),
    ElemElectric        UMETA(DisplayName = "Electric Element Type"),
    ElemEnergy        UMETA(DisplayName = "Energy Element Type"),
};

UENUM(BlueprintType)
enum EActionList {
    ActionNone       UMETA(DisplayName = "None"),
    ActionInteract       UMETA(DisplayName = "Interact"),
    ActionAttack       UMETA(DisplayName="Attack"),
    ActionHeavyAttack       UMETA(DisplayName = "Attack"),
    ActionUseItem      UMETA(DisplayName = "Use Item"),
    ActionMoveForward        UMETA(DisplayName="MoveForward"),
    ActionMoveRight        UMETA(DisplayName="MoveRight"),
    ActionDodge UMETA(DisplayName="Dodge"),
    ActionToggleAmpField UMETA(DisplayName="Toggle Amp Field"),
    ActionVentAmp UMETA(DisplayName="Vent Amp"),
};

UENUM(BlueprintType)
enum EAnimBPEvents {
    AnimInterrupt       UMETA(DisplayName = "Interrupt"),
    AnimDodge        UMETA(DisplayName = "Dodge"),
    AnimRotationRate        UMETA(DisplayName = "Rotation"),
};

UENUM(BlueprintType)
enum EMapExitType {
    Normal      UMETA(DisplayName = "Normal Map Exit"),
    BountyContract        UMETA(DisplayName = "Bounty Contract Map Exit"),
};


#pragma region data table structs

USTRUCT(BlueprintType)
struct FCommonSkeletalMesh : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USkeletalMesh* Mesh;
};

USTRUCT(BlueprintType)
struct FCommonMaterial : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkeletalMaterial Material;
};

#pragma endregion