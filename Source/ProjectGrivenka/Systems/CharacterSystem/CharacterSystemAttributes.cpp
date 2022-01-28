// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSystemAttributes.h"


float UCharacterSystemAttributes::PreAttributeChange(TEnumAsByte<EAttributeCode> Attribute, float NewValue)
{
    switch (Attribute)
    {
        case EAttributeCode::ATT_Health:
            this->Health.TempValue = this->Health.CurrentValue;
            return FMath::Clamp(NewValue, 0.0f, this->MaxHealth.CurrentValue);
        case EAttributeCode::ATT_Stamina:
            this->Stamina.TempValue = this->Stamina.CurrentValue;
            return FMath::Clamp(NewValue, 0.0f, this->MaxStamina.CurrentValue);
        case EAttributeCode::ATT_Amp:
            this->Amp.TempValue = this->Amp.CurrentValue;
            return FMath::Clamp(NewValue, 0.0f, this->MaxAmp.CurrentValue);
        case EAttributeCode::ATT_Fortitude:
            this->Fortitude.TempValue = this->Fortitude.CurrentValue;
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

void UCharacterSystemAttributes::PostAttributeChange(TEnumAsByte<EAttributeCode> Attribute)
{
    switch (Attribute) {
    case EAttributeCode::ATT_Health:
        if(this->Health.CurrentValue != this->Health.TempValue) this->HealthChangeDelegate.Broadcast();
        break;
    case EAttributeCode::ATT_Stamina:
        if (this->Stamina.CurrentValue != this->Stamina.TempValue) this->StaminaChangeDelegate.Broadcast();
        break;
    case EAttributeCode::ATT_Fortitude:
        if (this->Fortitude.CurrentValue != this->Fortitude.TempValue) this->FortitudeChangeDelegate.Broadcast();
        break;
    }
}
