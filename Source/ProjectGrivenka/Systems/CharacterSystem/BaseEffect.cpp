// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEffect.h"
#include "CharacterSystemAttributes.h"
#include "CharacterSystemAvailable.h"
#include "CharacterSystem.h"

//SPONGE: regen blocker & other overwrite Effect things need to handle a case where there's a stacking effects probably should add a pause/resume effect function
void UBaseEffect::Init(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FEffectInfo InEffectInfo)
{
	this->EffectInstigator = NewEffectInstigator;
	this->EffectReceiver = NewEffectReceiver;
	this->EffectInfo = InEffectInfo;
}

void UBaseEffect::UpdateEffectInfo(FEffectInfo InEffectInfo)
{
	//In case where an effect is generated inside initoverloaded & the effect value is based on effect receiver/instigator
	this->EffectInfo = InEffectInfo;
}

void UBaseEffect::OnActivated()
{
	this->OnPreExecuteEffect();
	if (this->EffectInfo.TimeType == EEffectTimeType::ETT_INSTANT) {
		this->OnExecuteEffect();
		this->RemoveSelf();
		return;
	}
	
	if (this->EffectInfo.TimeType == EEffectTimeType::ETT_TIMED) {
		this->EffectReceiver->GetWorld()->GetTimerManager().SetTimer(this->DurationHandler, this, &UBaseEffect::OnDurationTick, 1, true, -1);
	}
	else if (this->EffectInfo.TimeType == EEffectTimeType::ETT_INFINITE) {
		//Not really sure what to do here
	}

	if (this->EffectInfo.TickRate > 0) {
		this->EffectReceiver->GetWorld()->GetTimerManager().SetTimer(this->TickHandler, this, &UBaseEffect::OnTick, this->EffectInfo.TickRate, true, -1);
	}
	else {
		this->OnExecuteEffect();
	}

	GLog->Log("=============");
	for (int i = 0; i < ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver)->ActiveEffects.Num(); i++) {
		GLog->Log(ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver)->ActiveEffects[i]->EffectInfo.EffectId.ToString());
	}
	GLog->Log("=============");
}

void UBaseEffect::OnForceInterrupt() {
	this->RemoveSelf();
}

void UBaseEffect::OnDeactivated() {
	this->OnPostExecuteEffect();
	this->OnEnded();
}

void UBaseEffect::OnDurationTick()
{
	this->PooledDuration += 1;
	if (this->PooledDuration >= this->EffectInfo.Duration) {
		this->RemoveSelf();
	}
}

void UBaseEffect::OnTick()
{
	this->OnExecuteEffect();
}

void UBaseEffect::OnPreExecuteEffect()
{
}

void UBaseEffect::OnExecuteEffect()
{
	for (int i = 0; i < this->EffectInfo.EffectProcesses.Num(); i++) {
		FEffectProcess ExecutionProcess = this->EffectInfo.EffectProcesses[i];
		switch (ExecutionProcess.EffectOperator)
		{
			case EEffectOperationType::EOT_SUM:
				this->BasicSumExecute(ExecutionProcess.AffectedAttribute, ExecutionProcess.Value);
				break;
			case EEffectOperationType::EOT_MULTIPLY:
				this->BasicMultiplyExecute(ExecutionProcess.AffectedAttribute, ExecutionProcess.Value);
				break;
			case EEffectOperationType::EOT_OVERWRITE:
				this->BasicOverwriteExecute(ExecutionProcess.AffectedAttribute, ExecutionProcess.Value);
				break;
			default:
				break;
		}
	}
}

void UBaseEffect::OnPostExecuteEffect()
{
	//no revert changes on instant effects or effects that ticks
	if (this->EffectInfo.TimeType == EEffectTimeType::ETT_INSTANT || this->EffectInfo.TickRate > 0) return;

	for (int i = 0; i < this->EffectInfo.EffectProcesses.Num(); i++) {
		FEffectProcess ExecutionProcess = this->EffectInfo.EffectProcesses[i];
		switch (ExecutionProcess.EffectOperator)
		{
			case EEffectOperationType::EOT_SUM:
				this->RevertSumExecute(ExecutionProcess.AffectedAttribute, ExecutionProcess.Value);
				break;
			case EEffectOperationType::EOT_MULTIPLY:
				this->RevertMultiplyExecute(ExecutionProcess.AffectedAttribute, ExecutionProcess.Value);
				break;
			case EEffectOperationType::EOT_OVERWRITE:
				this->RevertOverwriteExecute(ExecutionProcess.AffectedAttribute, ExecutionProcess.Value);
				break;
			default:
				break;
		}
	}
}

void UBaseEffect::OnEnded()
{
	this->EffectReceiver->GetWorld()->GetTimerManager().ClearTimer(this->TickHandler);
	this->EffectReceiver->GetWorld()->GetTimerManager().ClearTimer(this->DurationHandler);
}

void UBaseEffect::RemoveSelf() {
	UCharacterSystem* CharacterSystemComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	if (!CharacterSystemComp) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Effect Parent"), *GetNameSafe(this));
		return;
	}
	CharacterSystemComp->RemoveEffect(this);
}

void UBaseEffect::BasicSumExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue)
{
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (InAttributeCode)
	{
	case EAttributeCode::ATT_Health:
		ReceiverComp->Attributes->SetHealth(ReceiverComp->Attributes->GetHealth() + InValue);
		break;
	case EAttributeCode::ATT_Stamina:
		ReceiverComp->Attributes->SetStamina(ReceiverComp->Attributes->GetStamina() + InValue);
		break;
	case EAttributeCode::ATT_Amp:
		ReceiverComp->Attributes->SetAmp(ReceiverComp->Attributes->GetAmp() + InValue);
		break;
	case EAttributeCode::ATT_Fortitude:
		ReceiverComp->Attributes->SetFortitude(ReceiverComp->Attributes->GetFortitude() + InValue);
		break;
	case EAttributeCode::ATT_MaxHealth:
		ReceiverComp->Attributes->SetMaxHealth(ReceiverComp->Attributes->GetMaxHealth() + InValue);
		break;
	case EAttributeCode::ATT_MaxStamina:
		ReceiverComp->Attributes->SetMaxStamina(ReceiverComp->Attributes->GetMaxStamina() + InValue);
		break;
	case EAttributeCode::ATT_MaxAmp:
		ReceiverComp->Attributes->SetMaxAmp(ReceiverComp->Attributes->GetMaxAmp() + InValue);
		break;
	case EAttributeCode::ATT_HealthRecoverRate:
		ReceiverComp->Attributes->SetHealthRecoverRate(ReceiverComp->Attributes->GetHealthRecoverRate() + InValue);
		break;
	case EAttributeCode::ATT_StaminaRecoverRate:
		ReceiverComp->Attributes->SetStaminaRecoverRate(ReceiverComp->Attributes->GetStaminaRecoverRate() + InValue);
		break;
	case EAttributeCode::ATT_AmpRecoverRate:
		ReceiverComp->Attributes->SetAmpRecoverRate(ReceiverComp->Attributes->GetAmpRecoverRate() + InValue);
		break;
	case EAttributeCode::ATT_FortitudeRecoverRate:
		ReceiverComp->Attributes->SetFortitudeRecoverRate(ReceiverComp->Attributes->GetFortitudeRecoverRate() + InValue);
		break;
	case EAttributeCode::ATT_WeaponDamage:
		ReceiverComp->Attributes->SetWeaponDamage(ReceiverComp->Attributes->GetWeaponDamage() + InValue);
		break;
	case EAttributeCode::ATT_WeaponMovingValues:
		ReceiverComp->Attributes->SetWeaponMovingValues(ReceiverComp->Attributes->GetWeaponMovingValues() + InValue);
		break;
	case EAttributeCode::ATT_Defense:
		ReceiverComp->Attributes->SetDefense(ReceiverComp->Attributes->GetDefense() + InValue);
		break;
	default:
		break;
	}
}

void UBaseEffect::BasicMultiplyExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue)
{
	//SPONGE: might need to take from base value cause of effect stacking issues
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (InAttributeCode)
	{
	case EAttributeCode::ATT_Health:
		ReceiverComp->Attributes->SetHealth(ReceiverComp->Attributes->GetHealth() * InValue);
		break;
	case EAttributeCode::ATT_Stamina:
		ReceiverComp->Attributes->SetStamina(ReceiverComp->Attributes->GetStamina() * InValue);
		break;
	case EAttributeCode::ATT_Amp:
		ReceiverComp->Attributes->SetAmp(ReceiverComp->Attributes->GetAmp() * InValue);
		break;
	case EAttributeCode::ATT_Fortitude:
		ReceiverComp->Attributes->SetFortitude(ReceiverComp->Attributes->GetFortitude() * InValue);
		break;
	case EAttributeCode::ATT_MaxHealth:
		ReceiverComp->Attributes->SetMaxHealth(ReceiverComp->Attributes->GetMaxHealth() * InValue);
		break;
	case EAttributeCode::ATT_MaxStamina:
		ReceiverComp->Attributes->SetMaxStamina(ReceiverComp->Attributes->GetMaxStamina() * InValue);
		break;
	case EAttributeCode::ATT_MaxAmp:
		ReceiverComp->Attributes->SetMaxAmp(ReceiverComp->Attributes->GetMaxAmp() * InValue);
		break;
	case EAttributeCode::ATT_HealthRecoverRate:
		ReceiverComp->Attributes->SetHealthRecoverRate(ReceiverComp->Attributes->GetHealthRecoverRate() * InValue);
		break;
	case EAttributeCode::ATT_StaminaRecoverRate:
		ReceiverComp->Attributes->SetStaminaRecoverRate(ReceiverComp->Attributes->GetStaminaRecoverRate() * InValue);
		break;
	case EAttributeCode::ATT_AmpRecoverRate:
		ReceiverComp->Attributes->SetAmpRecoverRate(ReceiverComp->Attributes->GetAmpRecoverRate() * InValue);
		break;
	case EAttributeCode::ATT_FortitudeRecoverRate:
		ReceiverComp->Attributes->SetFortitudeRecoverRate(ReceiverComp->Attributes->GetFortitudeRecoverRate() * InValue);
		break;
	case EAttributeCode::ATT_WeaponDamage:
		ReceiverComp->Attributes->SetWeaponDamage(ReceiverComp->Attributes->GetWeaponDamage() * InValue);
		break;
	case EAttributeCode::ATT_WeaponMovingValues:
		ReceiverComp->Attributes->SetWeaponMovingValues(ReceiverComp->Attributes->GetWeaponMovingValues() * InValue);
		break;
	case EAttributeCode::ATT_Defense:
		ReceiverComp->Attributes->SetDefense(ReceiverComp->Attributes->GetDefense() * InValue);
		break;
	default:
		break;
	}
}

void UBaseEffect::BasicOverwriteExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue)
{
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (InAttributeCode)
	{
	case EAttributeCode::ATT_Health:
		ReceiverComp->Attributes->SetHealth(InValue);
		break;
	case EAttributeCode::ATT_Stamina:
		ReceiverComp->Attributes->SetStamina(InValue);
		break;
	case EAttributeCode::ATT_Amp:
		ReceiverComp->Attributes->SetAmp(InValue);
		break;
	case EAttributeCode::ATT_Fortitude:
		ReceiverComp->Attributes->SetFortitude(InValue);
		break;
	case EAttributeCode::ATT_MaxHealth:
		ReceiverComp->Attributes->SetMaxHealth(InValue);
		break;
	case EAttributeCode::ATT_MaxStamina:
		ReceiverComp->Attributes->SetMaxStamina(InValue);
		break;
	case EAttributeCode::ATT_MaxAmp:
		ReceiverComp->Attributes->SetMaxAmp(InValue);
		break;
	case EAttributeCode::ATT_HealthRecoverRate:
		ReceiverComp->Attributes->SetHealthRecoverRate(InValue);
		break;
	case EAttributeCode::ATT_StaminaRecoverRate:
		ReceiverComp->Attributes->SetStaminaRecoverRate(InValue);
		break;
	case EAttributeCode::ATT_AmpRecoverRate:
		ReceiverComp->Attributes->SetAmpRecoverRate(InValue);
		break;
	case EAttributeCode::ATT_FortitudeRecoverRate:
		ReceiverComp->Attributes->SetFortitudeRecoverRate(InValue);
		break;
	case EAttributeCode::ATT_WeaponDamage:
		ReceiverComp->Attributes->SetWeaponDamage(InValue);
		break;
	case EAttributeCode::ATT_WeaponMovingValues:
		ReceiverComp->Attributes->SetWeaponMovingValues(InValue);
		break;
	case EAttributeCode::ATT_Defense:
		ReceiverComp->Attributes->SetDefense(InValue);
		break;
	default:
		break;
	}
}

void UBaseEffect::RevertSumExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue)
{
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (InAttributeCode)
	{
	case EAttributeCode::ATT_Health:
		ReceiverComp->Attributes->SetHealth(ReceiverComp->Attributes->GetHealth() - InValue);
		break;
	case EAttributeCode::ATT_Stamina:
		ReceiverComp->Attributes->SetStamina(ReceiverComp->Attributes->GetStamina() - InValue);
		break;
	case EAttributeCode::ATT_Amp:
		ReceiverComp->Attributes->SetAmp(ReceiverComp->Attributes->GetAmp() - InValue);
		break;
	case EAttributeCode::ATT_Fortitude:
		ReceiverComp->Attributes->SetFortitude(ReceiverComp->Attributes->GetFortitude() - InValue);
		break;
	case EAttributeCode::ATT_MaxHealth:
		ReceiverComp->Attributes->SetMaxHealth(ReceiverComp->Attributes->GetMaxHealth() - InValue);
		break;
	case EAttributeCode::ATT_MaxStamina:
		ReceiverComp->Attributes->SetMaxStamina(ReceiverComp->Attributes->GetMaxStamina() - InValue);
		break;
	case EAttributeCode::ATT_MaxAmp:
		ReceiverComp->Attributes->SetMaxAmp(ReceiverComp->Attributes->GetMaxAmp() - InValue);
		break;
	case EAttributeCode::ATT_HealthRecoverRate:
		ReceiverComp->Attributes->SetHealthRecoverRate(ReceiverComp->Attributes->GetHealthRecoverRate() - InValue);
		break;
	case EAttributeCode::ATT_StaminaRecoverRate:
		ReceiverComp->Attributes->SetStaminaRecoverRate(ReceiverComp->Attributes->GetStaminaRecoverRate() - InValue);
		break;
	case EAttributeCode::ATT_AmpRecoverRate:
		ReceiverComp->Attributes->SetAmpRecoverRate(ReceiverComp->Attributes->GetAmpRecoverRate() - InValue);
		break;
	case EAttributeCode::ATT_FortitudeRecoverRate:
		ReceiverComp->Attributes->SetFortitudeRecoverRate(ReceiverComp->Attributes->GetFortitudeRecoverRate() - InValue);
		break;
	case EAttributeCode::ATT_WeaponDamage:
		ReceiverComp->Attributes->SetWeaponDamage(ReceiverComp->Attributes->GetWeaponDamage() - InValue);
		break;
	case EAttributeCode::ATT_WeaponMovingValues:
		ReceiverComp->Attributes->SetWeaponMovingValues(ReceiverComp->Attributes->GetWeaponMovingValues() - InValue);
		break;
	case EAttributeCode::ATT_Defense:
		ReceiverComp->Attributes->SetDefense(ReceiverComp->Attributes->GetDefense() - InValue);
		break;
	default:
		break;
	}
}

void UBaseEffect::RevertMultiplyExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue)
{
	//SPONGE: might need to take from base value cause of effect stacking issues
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (InAttributeCode)
	{
	case EAttributeCode::ATT_Health:
		ReceiverComp->Attributes->SetHealth(ReceiverComp->Attributes->GetHealth() / InValue);
		break;
	case EAttributeCode::ATT_Stamina:
		ReceiverComp->Attributes->SetStamina(ReceiverComp->Attributes->GetStamina() / InValue);
		break;
	case EAttributeCode::ATT_Amp:
		ReceiverComp->Attributes->SetAmp(ReceiverComp->Attributes->GetAmp() / InValue);
		break;
	case EAttributeCode::ATT_Fortitude:
		ReceiverComp->Attributes->SetFortitude(ReceiverComp->Attributes->GetFortitude() / InValue);
		break;
	case EAttributeCode::ATT_MaxHealth:
		ReceiverComp->Attributes->SetMaxHealth(ReceiverComp->Attributes->GetMaxHealth() / InValue);
		break;
	case EAttributeCode::ATT_MaxStamina:
		ReceiverComp->Attributes->SetMaxStamina(ReceiverComp->Attributes->GetMaxStamina() / InValue);
		break;
	case EAttributeCode::ATT_MaxAmp:
		ReceiverComp->Attributes->SetMaxAmp(ReceiverComp->Attributes->GetMaxAmp() / InValue);
		break;
	case EAttributeCode::ATT_HealthRecoverRate:
		ReceiverComp->Attributes->SetHealthRecoverRate(ReceiverComp->Attributes->GetHealthRecoverRate() / InValue);
		break;
	case EAttributeCode::ATT_StaminaRecoverRate:
		ReceiverComp->Attributes->SetStaminaRecoverRate(ReceiverComp->Attributes->GetStaminaRecoverRate() / InValue);
		break;
	case EAttributeCode::ATT_AmpRecoverRate:
		ReceiverComp->Attributes->SetAmpRecoverRate(ReceiverComp->Attributes->GetAmpRecoverRate() / InValue);
		break;
	case EAttributeCode::ATT_FortitudeRecoverRate:
		ReceiverComp->Attributes->SetFortitudeRecoverRate(ReceiverComp->Attributes->GetFortitudeRecoverRate() / InValue);
		break;
	case EAttributeCode::ATT_WeaponDamage:
		ReceiverComp->Attributes->SetWeaponDamage(ReceiverComp->Attributes->GetWeaponDamage() / InValue);
		break;
	case EAttributeCode::ATT_WeaponMovingValues:
		ReceiverComp->Attributes->SetWeaponMovingValues(ReceiverComp->Attributes->GetWeaponMovingValues() / InValue);
		break;
	case EAttributeCode::ATT_Defense:
		ReceiverComp->Attributes->SetDefense(ReceiverComp->Attributes->GetDefense() / InValue);
		break;
	default:
		break;
	}
}

void UBaseEffect::RevertOverwriteExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue)
{
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (InAttributeCode)
	{
	case EAttributeCode::ATT_Health:
		ReceiverComp->Attributes->SetHealth(ReceiverComp->Attributes->Health.BaseValue);
		break;
	case EAttributeCode::ATT_Stamina:
		ReceiverComp->Attributes->SetStamina(ReceiverComp->Attributes->Stamina.BaseValue);
		break;
	case EAttributeCode::ATT_Amp:
		ReceiverComp->Attributes->SetAmp(ReceiverComp->Attributes->Amp.BaseValue);
		break;
	case EAttributeCode::ATT_Fortitude:
		ReceiverComp->Attributes->SetFortitude(ReceiverComp->Attributes->Fortitude.BaseValue);
		break;
	case EAttributeCode::ATT_MaxHealth:
		ReceiverComp->Attributes->SetMaxHealth(ReceiverComp->Attributes->MaxHealth.BaseValue);
		break;
	case EAttributeCode::ATT_MaxStamina:
		ReceiverComp->Attributes->SetMaxStamina(ReceiverComp->Attributes->MaxStamina.BaseValue);
		break;
	case EAttributeCode::ATT_MaxAmp:
		ReceiverComp->Attributes->SetMaxAmp(ReceiverComp->Attributes->MaxAmp.BaseValue);
		break;
	case EAttributeCode::ATT_HealthRecoverRate:
		ReceiverComp->Attributes->SetHealthRecoverRate(ReceiverComp->Attributes->HealthRecoverRate.BaseValue);
		break;
	case EAttributeCode::ATT_StaminaRecoverRate:
		ReceiverComp->Attributes->SetStaminaRecoverRate(ReceiverComp->Attributes->StaminaRecoverRate.BaseValue);
		break;
	case EAttributeCode::ATT_AmpRecoverRate:
		ReceiverComp->Attributes->SetAmpRecoverRate(ReceiverComp->Attributes->AmpRecoverRate.BaseValue);
		break;
	case EAttributeCode::ATT_FortitudeRecoverRate:
		ReceiverComp->Attributes->SetFortitudeRecoverRate(ReceiverComp->Attributes->FortitudeRecoverRate.BaseValue);
		break;
	case EAttributeCode::ATT_WeaponDamage:
		ReceiverComp->Attributes->SetWeaponDamage(ReceiverComp->Attributes->WeaponDamage.BaseValue);
		break;
	case EAttributeCode::ATT_WeaponMovingValues:
		ReceiverComp->Attributes->SetWeaponMovingValues(ReceiverComp->Attributes->WeaponMovingValues.BaseValue);
		break;
	case EAttributeCode::ATT_Defense:
		ReceiverComp->Attributes->SetDefense(ReceiverComp->Attributes->Defense.BaseValue);
		break;
	default:
		break;
	}
}

