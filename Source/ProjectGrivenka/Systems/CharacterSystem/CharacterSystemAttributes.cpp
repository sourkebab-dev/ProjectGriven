// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSystemAttributes.h"


float UCharacterSystemAttributes::PreAttributeChange(TEnumAsByte<EAttributeCode> Attribute, float NewValue)
{
    switch (Attribute)
    {
        case EAttributeCode::ATT_Health:
            return FMath::Clamp(NewValue, 0.0f, this->MaxHealth.CurrentValue);
        case EAttributeCode::ATT_Stamina:
            return FMath::Clamp(NewValue, 0.0f, this->MaxStamina.CurrentValue);
        case EAttributeCode::ATT_Amp:
            return FMath::Clamp(NewValue, 0.0f, this->MaxAmp.CurrentValue);
        case EAttributeCode::ATT_Fortitude:
            return FMath::Clamp(NewValue, 0.0f, this->MaxFortitude.CurrentValue);
        case EAttributeCode::ATT_ElemFirePooledDamage:
            return FMath::Clamp(NewValue, 0.0f, 100.0f);
        case EAttributeCode::ATT_ElemIcePooledDamage:
            return FMath::Clamp(NewValue, 0.0f, 100.0f);
        case EAttributeCode::ATT_ElemElectricPooledDamage:
            return FMath::Clamp(NewValue, 0.0f, 100.0f);
        case EAttributeCode::ATT_ElemEnergyPooledDamage:
            return FMath::Clamp(NewValue, 0.0f, 100.0f);
        case EAttributeCode::ATT_ElemPoisonPooledDamage:
            return FMath::Clamp(NewValue, 0.0f, 100.0f);
        default:
            return NewValue;
    }


}
