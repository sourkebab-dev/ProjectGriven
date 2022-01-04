// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSystemDefinitions.h"
#include "UObject/NoExportTypes.h"
#include "CharacterSystemAttributes.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(AttributeName) \
	FORCEINLINE float Get##AttributeName() const { return AttributeName##.CurrentValue; } \
	FORCEINLINE void Set##AttributeName(float NewVal) { AttributeName##.CurrentValue = PreAttributeChange(EAttributeCode::ATT_##AttributeName##, NewVal); } \
	FORCEINLINE void Init##AttributeName(float NewVal){ AttributeName##.CurrentValue = NewVal; AttributeName##.BaseValue = NewVal; } 

UCLASS()
class PROJECTGRIVENKA_API UCharacterSystemAttributes : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
		FAttributeData MaxAmp;
		ATTRIBUTE_ACCESSORS(MaxAmp)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
		FAttributeData Amp;
		ATTRIBUTE_ACCESSORS(Amp)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amp")
		FAttributeData AmpRecoverRate;
		ATTRIBUTE_ACCESSORS(AmpRecoverRate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		FAttributeData MaxHealth;
		ATTRIBUTE_ACCESSORS(MaxHealth)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		FAttributeData Health;
		ATTRIBUTE_ACCESSORS(Health)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		FAttributeData HealthRecoverRate;
		ATTRIBUTE_ACCESSORS(HealthRecoverRate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		FAttributeData MaxStamina;
		ATTRIBUTE_ACCESSORS(MaxStamina)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		FAttributeData Stamina;
		ATTRIBUTE_ACCESSORS(Stamina)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		FAttributeData StaminaRecoverRate;
		ATTRIBUTE_ACCESSORS(StaminaRecoverRate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
		FAttributeData MaxFortitude;
		ATTRIBUTE_ACCESSORS(MaxFortitude)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
		FAttributeData Fortitude;
		ATTRIBUTE_ACCESSORS(Fortitude)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fortitude")
		FAttributeData FortitudeRecoverRate;
		ATTRIBUTE_ACCESSORS(FortitudeRecoverRate)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
		FAttributeData Defense;
		ATTRIBUTE_ACCESSORS(Defense)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FAttributeData WeaponDamage;
		ATTRIBUTE_ACCESSORS(WeaponDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FAttributeData ElementalDamage;
		ATTRIBUTE_ACCESSORS(ElementalDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FAttributeData FortitudeDamage;
		ATTRIBUTE_ACCESSORS(FortitudeDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemFireDefense;
		ATTRIBUTE_ACCESSORS(ElemFireDefense)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemIceDefense;
		ATTRIBUTE_ACCESSORS(ElemIceDefense)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemElectricDefense;
		ATTRIBUTE_ACCESSORS(ElemElectricDefense)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemPoisonDefense;
		ATTRIBUTE_ACCESSORS(ElemPoisonDefense)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemEnergyDefense;
		ATTRIBUTE_ACCESSORS(ElemEnergyDefense)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemFirePooledDamage;
		ATTRIBUTE_ACCESSORS(ElemFirePooledDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemIcePooledDamage;
		ATTRIBUTE_ACCESSORS(ElemIcePooledDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemElectricPooledDamage;
		ATTRIBUTE_ACCESSORS(ElemElectricPooledDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemEnergyPooledDamage;
		ATTRIBUTE_ACCESSORS(ElemEnergyPooledDamage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elemental")
		FAttributeData ElemPoisonPooledDamage;
		ATTRIBUTE_ACCESSORS(ElemPoisonPooledDamage)


	float PreAttributeChange(TEnumAsByte<EAttributeCode> Attribute, float NewValue);
};
