// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CharacterSystemDefinitions.generated.h"

UENUM(BlueprintType)
enum EAttributeCode {
    ATT_Health       UMETA(DisplayName = "Health Attribute"),
    ATT_MaxHealth       UMETA(DisplayName = "Max Health Attribute"),
    ATT_HealthRecoverRate UMETA(DisplayName = "Health Recover Rate Attribute"),
    ATT_Amp       UMETA(DisplayName = "Amp Attribute"),
    ATT_MaxAmp       UMETA(DisplayName = "Max Amp Attribute"),
    ATT_AmpRecoverRate UMETA(DisplayName = "Amp Recover Rate Attribute"),
    ATT_Stamina       UMETA(DisplayName = "Stamina Attribute"),
    ATT_MaxStamina       UMETA(DisplayName = "Max Stamina Attribute"),
    ATT_StaminaRecoverRate UMETA(DisplayName = "Stamina Recover Rate Attribute"),
    ATT_Fortitude       UMETA(DisplayName = "Fortitude Attribute"),
    ATT_MaxFortitude UMETA(DisplayName = "Max Fortitude Attribute"),
    ATT_FortitudeRecoverRate UMETA(DisplayName = "Fortitude Recover Rate Attribute"),
    ATT_Defense       UMETA(DisplayName = "Defense Attribute"),
    ATT_CriticalPower     UMETA(DisplayName = "Critical Power Attribute"),
    ATT_CriticalChance     UMETA(DisplayName = "Critical Chance Attribute"),
    ATT_WeaponDamage       UMETA(DisplayName = "Weapon Damage Attribute"),
    ATT_ElementalDamage     UMETA(DisplayName = "Weapon Elemental Damage Attribute"),
    ATT_ElemFireDefense     UMETA(DisplayName = "Fire Defense Attribute"),
    ATT_ElemIceDefense      UMETA(DisplayName = "Ice Defense Attribute"),
    ATT_ElemElectricDefense     UMETA(DisplayName = "Electric Defense Attribute"),
    ATT_ElemPoisonDefense       UMETA(DisplayName = "Poison Defense Attribute"),
    ATT_ElemEnergyDefense       UMETA(DisplayName = "Energy Defense Attribute"),
    ATT_ElemFirePooledDamage    UMETA(DisplayName = "Fire Pooled Damage Attribute"),
    ATT_ElemIcePooledDamage     UMETA(DisplayName = "Ice Pooled Damage Attribute"),
    ATT_ElemElectricPooledDamage        UMETA(DisplayName = "Electric Pooled Damage Attribute"),
    ATT_ElemEnergyPooledDamage      UMETA(DisplayName = "Energy Pooled Damage Attribute"),
    ATT_ElemPoisonPooledDamage      UMETA(DisplayName = "Poison Pooled Damage Attribute"),
};

UENUM(BlueprintType)
enum EEffectTimeType {
    ETT_INSTANT       UMETA(DisplayName = "Instant Active"),
    ETT_INFINITE       UMETA(DisplayName = "Infinite Active"),
    ETT_TIMED       UMETA(DisplayName = "Timed Active"),
};

UENUM(BlueprintType)
enum EEffectExecutionType {
    EET_BASIC       UMETA(DisplayName = "Basic Effect Calculation"),
    EET_CUSTOM       UMETA(DisplayName = "Custom Effect Calculation"),
};

UENUM(BlueprintType)
enum EEffectOperationType {
    EOT_SUM       UMETA(DisplayName = "Sum of Value"),
    EOT_MULTIPLY       UMETA(DisplayName = "Value Multiplication"),
    EOT_OVERWRITE       UMETA(DisplayName = "Value Overwrite"),
};

USTRUCT(BlueprintType)
struct FAttributeData
{
    GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float BaseValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float CurrentValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FEffectProcess
{
    GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<EAttributeCode> AffectedAttribute;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<EEffectOperationType> EffectOperator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float Value;
};

USTRUCT(BlueprintType)
struct FEffectInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        FName EffectId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        FName EffectName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        FText EffectDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        UTexture2D* EffectIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        FName EffectInstigatorName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        FGameplayTagContainer EffectTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
        FGameplayTagContainer CancelledEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
        TEnumAsByte<EEffectTimeType> TimeType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
        float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
        float TickRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Process")
        TArray<FEffectProcess> EffectProcesses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Process")
        TEnumAsByte<EEffectExecutionType> EffectExecutionType;

};