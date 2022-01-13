// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDamage.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

//Sponge: might need to rename this to add damage, might also need to rethink where to store weapon damage and all that shit also need to implement pooled elemental damage
void UWeaponDamage::InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FDamageInfo InDamageInfo) {
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef("Util_WeaponHit");
	if (!EffectPrefab) return;
	FEffectInfo TempEffectInfo = EffectPrefab->EffectInfo;
	this->DamageInfo = InDamageInfo;
	Super::Init(NewEffectInstigator, NewEffectReceiver, TempEffectInfo);
}

void UWeaponDamage::OnExecuteEffect() {
	//if (this->EffectReceiverCharacter && this->EffectReceiverCharacter->CurrentStateCode == ECharacterStates::StateDodge) return;

	//Health Damage Calculation
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	UCharacterSystem* InstigatorComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectInstigator);

	float ReceiverDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Defense);
	float ReceiverHealth = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Health);
	float ReceiverFortude = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Fortitude);
	float ReceiverAmp = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Amp);
	float ReceiverDamageAbsorption = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_DamageAbsorption);
	float ReceiverStamina = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Stamina);
	float ReceiverElementalDefense = 0.0;

	GLog->Log("DamageAbsr");
	GLog->Log(FString::SanitizeFloat(ReceiverDamageAbsorption));

	switch (this->DamageInfo.ElementType) {
		case EDamageElementType::ElemElectric:
			ReceiverElementalDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_ElemElectricDefense);
			break;
		case EDamageElementType::ElemEnergy:
			ReceiverElementalDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_ElemEnergyDefense);
			break;
		case EDamageElementType::ElemFire:
			ReceiverElementalDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_ElemFireDefense);
			break;
		case EDamageElementType::ElemIce:
			ReceiverElementalDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_ElemIceDefense);
			break;
		case EDamageElementType::ElemPoison:
			ReceiverElementalDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_ElemPoisonDefense);
			break;
		default:
			break;
	}

	float RawDamage = this->DamageInfo.RawPhysicalDamage;
	float RawElementalDamage = this->DamageInfo.RawElementalDamage;
	float CalculatedPhysicalDamage = 0.0;
	float CalculatedElementalDamage = 0.0;
	if (!this->DamageInfo.IsFixed) {
		float InstigatorCritPower = InstigatorComp->GetAttributeCurrentValue(EAttributeCode::ATT_CriticalPower);
		float InstigatorCritChance = InstigatorComp->GetAttributeCurrentValue(EAttributeCode::ATT_CriticalChance);
		float MovingValues = this->DamageInfo.MovingValues;
		float CritRand = FMath::RandRange(0, 100);
		bool isCritical = CritRand <= InstigatorCritChance;

		float DamageByMovement = RawDamage * (MovingValues / 100);
		float ElemDamageByMovement = RawElementalDamage * (MovingValues / 100);
		CalculatedPhysicalDamage = isCritical ? DamageByMovement + InstigatorCritPower : DamageByMovement;
		CalculatedElementalDamage = isCritical ? ElemDamageByMovement + InstigatorCritPower : ElemDamageByMovement;
	}
	else {
		CalculatedPhysicalDamage = RawDamage;
		CalculatedElementalDamage = RawElementalDamage;
	}
	
	//Fortitude Damage 
	//sponge: need to reset by debounce need to recheck the numbers
	float FortitudeDamage = 0.0;
	switch (this->DamageInfo.ImpactType)
	{
		case EDamageImpactType::DI_LOW:
			FortitudeDamage = 50;
			break;
		case EDamageImpactType::DI_MEDIUM:
			FortitudeDamage = 100;
			break;
		case EDamageImpactType::DI_HIGH:
			FortitudeDamage = 200;
			break;
		default:
			break;
	}
	
	//sponge: might need to make a common func for defense calculation
	float TotalPhysicalDamage = CalculatedPhysicalDamage ? CalculatedPhysicalDamage * (CalculatedPhysicalDamage / (CalculatedPhysicalDamage + ReceiverDefense)) : 0.0;
	float TotalElementalDamage = CalculatedElementalDamage > 0.0 ? CalculatedElementalDamage * (CalculatedElementalDamage / (CalculatedElementalDamage + ReceiverElementalDefense)) : 0.0;
	
	//Shield Damage (elementals are not negated by shield)
	TotalPhysicalDamage = TotalPhysicalDamage - (TotalPhysicalDamage * (ReceiverDamageAbsorption / 100));
	FortitudeDamage = FortitudeDamage - (FortitudeDamage * (ReceiverDamageAbsorption / 100));
	float AccumulatedDamage = TotalPhysicalDamage + TotalElementalDamage;
	
	if (ReceiverComp->FindEffectsByTag(FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Block"))) {
		//Sponge: need to recheck numbers
		ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Stamina, ReceiverStamina - FortitudeDamage);
	}

	//sponge: might need amp def
	ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Health, ReceiverHealth - AccumulatedDamage);
	ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Fortitude, ReceiverFortude - FortitudeDamage);
	GLog->Log("ReceiverHealth");
	GLog->Log(FString::SanitizeFloat(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Health)));
}