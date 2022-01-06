// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSystem.h"
#include "CharacterSystemAttributes.h"
#include "BaseEffect.h"
#include "EffectPrefab.h"
#include "Effects/AttributeRegenOvertime.h"
#include "Effects/DepleteStamina.h"
#include "Effects/WeaponDamage.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"


// Called when the game starts
void UCharacterSystem::BeginPlay()
{
	Super::BeginPlay();
	this->Attributes = NewObject<UCharacterSystemAttributes>();
}

void UCharacterSystem::Init()
{
	Super::Init();
	if (!this->CompContext.EventBus) return;
	this->CompContext.EventBus->DamagedDelegate.AddDynamic(this, &UCharacterSystem::InitEffectReceiveHit);
	//this->CompContext.EventBus->ItemUsageDelegate.AddUObject(this, &UCharacterSystem::InitFXByPrefab);
	if (this->PrefabData) this->InitializeAttributes(this->PrefabData->CharacterData.Attributes);
}

void UCharacterSystem::InitializeAttributes(FPersistedAttributes PersistedAttributes)
{
	this->Attributes->InitHealth(PersistedAttributes.Health);
	this->Attributes->InitMaxHealth(PersistedAttributes.MaxHealth);
	this->Attributes->InitHealthRecoverRate(PersistedAttributes.HealthRecoverRate);
	this->Attributes->InitAmp(PersistedAttributes.Amp);
	this->Attributes->InitMaxAmp(PersistedAttributes.MaxAmp);
	this->Attributes->InitAmpRecoverRate(PersistedAttributes.AmpRecoverRate);
	this->Attributes->InitStamina(PersistedAttributes.Stamina);
	this->Attributes->InitMaxStamina(PersistedAttributes.MaxStamina);
	this->Attributes->InitStaminaRecoverRate(PersistedAttributes.StaminaRecoverRate);
	this->Attributes->InitFortitude(PersistedAttributes.Fortitude);
	this->Attributes->InitMaxFortitude(PersistedAttributes.MaxFortitude);
	this->Attributes->InitFortitudeRecoverRate(PersistedAttributes.FortitudeRecoverRate);
	this->Attributes->InitWeaponDamage(PersistedAttributes.WeaponDamage);
	this->Attributes->InitDefense(PersistedAttributes.Defense);
	this->Attributes->InitElemFireDefense(PersistedAttributes.ElemFireDefense);
	this->Attributes->InitElemIceDefense(PersistedAttributes.ElemIceDefense);
	this->Attributes->InitElemElectricDefense(PersistedAttributes.ElemElectricDefense);
	this->Attributes->InitElemPoisonDefense(PersistedAttributes.ElemPoisonDefense);
	this->Attributes->InitElemEnergyDefense(PersistedAttributes.ElemEnergyDefense);


	//SPONGE: default regen initter, maybe got better solution?
	UAttributeRegenOvertime* HealthRegen = NewObject<UAttributeRegenOvertime>();
	HealthRegen->InitOverloaded(this->GetOwner(), this->GetOwner(), "Util_HealthRegen");
	UAttributeRegenOvertime* StaminaRegen = NewObject<UAttributeRegenOvertime>();
	StaminaRegen->InitOverloaded(this->GetOwner(), this->GetOwner(), "Util_StaminaRegen");
	UAttributeRegenOvertime* AmpRegen = NewObject<UAttributeRegenOvertime>();
	AmpRegen->InitOverloaded(this->GetOwner(), this->GetOwner(), "Util_AmpRegen");
	UAttributeRegenOvertime* FortitudeRegen = NewObject<UAttributeRegenOvertime>();
	FortitudeRegen->InitOverloaded(this->GetOwner(), this->GetOwner(), "Util_FortitudeRegen");
	this->AddEffect(HealthRegen);
	this->AddEffect(StaminaRegen);
	this->AddEffect(AmpRegen);
	this->AddEffect(FortitudeRegen);
}

void UCharacterSystem::AddEffect(UBaseEffect* InEffect)
{
	if (!InEffect->EffectInfo.CancelledEffects.IsEmpty()) {
		for (int i = 0; i < this->ActiveEffects.Num(); i++)
		{
			//SPONGE: PROBABLY NEED TO RECHECK THIS
			bool TagMatch = InEffect->EffectInfo.CancelledEffects.DoesTagContainerMatch(
				this->ActiveEffects[i]->EffectInfo.EffectTags,
				EGameplayTagMatchType::Explicit,
				EGameplayTagMatchType::Explicit,
				EGameplayContainerMatchType::Any
			);
			if (TagMatch) {
				this->RemoveEffect(this->ActiveEffects[i]);
			}
		}
	}

	this->ActiveEffects.Add(InEffect);
	InEffect->OnActivated();
}

void UCharacterSystem::RemoveEffect(UBaseEffect* InEffect)
{
	InEffect->OnDeactivated();
	this->ActiveEffects.Remove(InEffect);
}

void UCharacterSystem::RemoveEffectsByTag(FGameplayTag EffectTag)
{
	for (int i = 0; i < this->ActiveEffects.Num() ; i++)
	{
		UBaseEffect* Effect = this->ActiveEffects[i];
		if (Effect->EffectInfo.EffectTags.HasTag(EffectTag))
		{
			this->RemoveEffect(Effect);
		}
	}
}

void UCharacterSystem::RemoveEffectsById(FName EffectId)
{
	for (int i = 0; i < this->ActiveEffects.Num(); i++)
	{
		UBaseEffect* Effect = this->ActiveEffects[i];
		if (Effect->EffectInfo.EffectId == EffectId)
		{
			this->RemoveEffect(Effect);
		}
	}
}


void UCharacterSystem::DeactivateAllEffects()
{
	for (int i = 0; i < this->ActiveEffects.Num(); i++)
	{
		UBaseEffect* Effect = this->ActiveEffects[i];
		Effect->OnDeactivated();
	}
}

float UCharacterSystem::GetAttributeBaseValue(TEnumAsByte<EAttributeCode> InAttributeCode)
{
	switch (InAttributeCode) {
		case EAttributeCode::ATT_Amp:
			return this->Attributes->Amp.BaseValue;
		case EAttributeCode::ATT_Health:
			return this->Attributes->Health.BaseValue;
		case EAttributeCode::ATT_Stamina:
			return this->Attributes->Stamina.BaseValue;
		case EAttributeCode::ATT_Fortitude:
			return this->Attributes->Fortitude.BaseValue;
		default:
			return 0.0f;
	}
}

float UCharacterSystem::GetAttributeMaxValue(TEnumAsByte<EAttributeCode> InAttributeCode)
{
	switch (InAttributeCode) {
		case EAttributeCode::ATT_Amp:
			return this->Attributes->GetMaxAmp();
		case EAttributeCode::ATT_Health:
			return this->Attributes->GetMaxHealth();
		case EAttributeCode::ATT_Stamina:
			return this->Attributes->GetMaxStamina();
		case EAttributeCode::ATT_Fortitude:
			return this->Attributes->GetMaxFortitude();
		default:
			return 0.0f;
	}
}

float UCharacterSystem::GetAttributeCurrentValue(TEnumAsByte<EAttributeCode> InAttributeCode)
{
	//sponge: Maybe all this switch cases needs to be a map?
	switch (InAttributeCode) {
		case EAttributeCode::ATT_Amp:
			return this->Attributes->GetAmp();
		case EAttributeCode::ATT_AmpRecoverRate:
			return this->Attributes->GetAmpRecoverRate();
		case EAttributeCode::ATT_Health:
			return this->Attributes->GetHealth();
		case EAttributeCode::ATT_HealthRecoverRate:
			return this->Attributes->GetHealthRecoverRate();
		case EAttributeCode::ATT_Stamina:
			return this->Attributes->GetStamina();
		case EAttributeCode::ATT_StaminaRecoverRate:
			return this->Attributes->GetStaminaRecoverRate();
		case EAttributeCode::ATT_Fortitude:
			return this->Attributes->GetFortitude();
		case EAttributeCode::ATT_FortitudeRecoverRate:
			return this->Attributes->GetFortitudeRecoverRate();
		case EAttributeCode::ATT_CriticalPower:
			return this->Attributes->GetCriticalPower();
		case EAttributeCode::ATT_CriticalChance:
			return this->Attributes->GetCriticalChance();
		case EAttributeCode::ATT_WeaponDamage:
			return this->Attributes->GetWeaponDamage();
		case EAttributeCode::ATT_Defense:
			return this->Attributes->GetDefense();
		case EAttributeCode::ATT_ElemElectricDefense:
			return this->Attributes->GetElemElectricDefense();
		case EAttributeCode::ATT_ElemEnergyDefense:
			return this->Attributes->GetElemEnergyDefense();
		case EAttributeCode::ATT_ElemFireDefense:
			return this->Attributes->GetElemFireDefense();
		case EAttributeCode::ATT_ElemIceDefense:
			return this->Attributes->GetElemIceDefense();
		case EAttributeCode::ATT_ElemPoisonDefense:
			return this->Attributes->GetElemPoisonDefense();
		default:
			return 0.0f;
	}
}

void UCharacterSystem::SetAttributeValue(TEnumAsByte<EAttributeCode> InAttributeCode, float Value)
{
	switch (InAttributeCode) {
		case EAttributeCode::ATT_Amp:
			this->Attributes->SetAmp(Value);
			break;
		case EAttributeCode::ATT_Health:
			this->Attributes->SetHealth(Value);
			break;
		case EAttributeCode::ATT_Stamina:
			this->Attributes->SetStamina(Value);
			break;
		case EAttributeCode::ATT_Fortitude:
			this->Attributes->SetFortitude(Value);
			break;
		case EAttributeCode::ATT_WeaponDamage:
			this->Attributes->SetWeaponDamage(Value);
			break;
		case EAttributeCode::ATT_Defense:
			this->Attributes->SetDefense(Value);
			break;
		default:
			break;
	}
}

void UCharacterSystem::InitEffectByPrefab(AActor* EffectInstigator, UEffectPrefab* InEffectPrefab, float InOverrideValue, bool IsValueOverridden)
{
	UBaseEffect* EffectIns = NewObject<UBaseEffect>();
	FEffectInfo EffectInfo = InEffectPrefab->EffectInfo;
	if (IsValueOverridden) EffectInfo.EffectProcesses[0].Value = InOverrideValue;
	EffectIns->Init(EffectInstigator, this->GetOwner(), EffectInfo);
	this->AddEffect(EffectIns);
}

void UCharacterSystem::InitEffectByPrefabName(AActor* EffectInstigator, FName InPrefabName, float InOverrideValue, bool IsValueOverridden)
{
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef(InPrefabName);
	if (!EffectPrefab) return;
	FEffectInfo EffectInfo = EffectPrefab->EffectInfo;
	if (IsValueOverridden) EffectInfo.EffectProcesses[0].Value = InOverrideValue;
	UBaseEffect* NewEffect = NewObject<UBaseEffect>();
	NewEffect->Init(EffectInstigator, this->GetOwner(), EffectInfo);
	this->AddEffect(NewEffect);
}

void UCharacterSystem::InitEffectByRegenName(AActor* EffectInstigator, FName InPrefabName)
{
	UAttributeRegenOvertime* AmpRegen = NewObject<UAttributeRegenOvertime>();
	AmpRegen->InitOverloaded(EffectInstigator, this->GetOwner(), InPrefabName);
	this->AddEffect(AmpRegen);
}

void UCharacterSystem::InitEffectDepleteStamina(AActor* EffectInstigator, float InValue)
{
	UDepleteStamina* SpendStamina = NewObject<UDepleteStamina>();
	SpendStamina->InitOverloaded(EffectInstigator, this->GetOwner(), InValue);
	this->AddEffect(SpendStamina);
}

void UCharacterSystem::InitEffectReceiveHit(AActor* EffectInstigator, FDamageInfo InDamageInfo)
{
	UWeaponDamage* HitDamage = NewObject<UWeaponDamage>();
	HitDamage->InitOverloaded(EffectInstigator, this->GetOwner(), InDamageInfo);
	this->AddEffect(HitDamage);
}




