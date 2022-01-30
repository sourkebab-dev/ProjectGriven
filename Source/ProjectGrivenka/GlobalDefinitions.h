// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GlobalDefinitions.generated.h"

DECLARE_DYNAMIC_DELEGATE(FSimpleDynamicDelegate);


UENUM(BlueprintType)
enum EAIStateType
{
    IDLE,
    PATROL,
    SEARCH,
    COMBAT,
};

UENUM(BlueprintType)
enum EHostilityType
{
    WILD,
    HOSTILE,
    NEUTRAL,
    ALLY,
};

UENUM(BlueprintType)
enum EDamageElementType
{
    ElemNone UMETA(DisplayName = "No Element Type"),
    ElemFire       UMETA(DisplayName = "Fire Element Type"),
    ElemPoison       UMETA(DisplayName = "Poison Element Type"),
    ElemIce        UMETA(DisplayName = "Ice Element Type"),
    ElemElectric        UMETA(DisplayName = "Electric Element Type"),
    ElemEnergy        UMETA(DisplayName = "Energy Element Type"),
};

UENUM(BlueprintType)
enum EAttackMovementType {
    AM_DEFAULT       UMETA(DisplayName = "Default Attack Movement"),
    AM_HEAVY       UMETA(DisplayName = "Heavy Attack Movement"),
    AM_CHARGED       UMETA(DisplayName = "Charged Attack Movement"),
    AM_LUNGE       UMETA(DisplayName = "Lunge Attack Movement"),
};

UENUM(BlueprintType)
enum EDamageImpactType {
    DI_LOW       UMETA(DisplayName = "Low Impact"),
    DI_MEDIUM       UMETA(DisplayName = "Medium Impact"),
    DI_HIGH       UMETA(DisplayName = "High Impact"),
    DI_EXPLOSIVE       UMETA(DisplayName = "Explosive Impact"),
};


UENUM(BlueprintType)
enum EAICommandType {
    ATTACK,
    DEFEND,
    MOVETO,
    INTERACT,
};

USTRUCT(BlueprintType)
struct FCommandInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EAICommandType> CommandType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        AActor* CommandTargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FVector CommandTargetLocation;
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
struct FDamageInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RawPhysicalDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RawElementalDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsFixed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsAbsorbed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MovingValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EDamageImpactType> ImpactType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EDamageElementType> ElementType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector DamageDirection; // 0.0.0 for radial
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

UENUM(BlueprintType)
enum EActionList {
    ActionNone       UMETA(DisplayName = "None"),
    ActionInteract       UMETA(DisplayName = "Interact"),
    ActionAttack       UMETA(DisplayName="Attack"),
    ActionUseItem      UMETA(DisplayName = "Use Item"),
    ActionMoveForward        UMETA(DisplayName="MoveForward"),
    ActionMoveRight        UMETA(DisplayName="MoveRight"),
    ActionDodge     UMETA(DisplayName="Dodge"),
    ActionToggleAmpField UMETA(DisplayName="Toggle Amp Field"),
    ActionVentAmp   UMETA(DisplayName="Vent Amp"),
    ActionBlock     UMETA(DisplayName = "Block"),
    ActionCommand  UMETA(DisplayName = "Command"),
    ActionCommandCancel UMETA(DisplayName = "Command Cancel"),
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