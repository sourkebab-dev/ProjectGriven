// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDamage.h"
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

	GLog->Log("RawDamage");
	GLog->Log(FString::SanitizeFloat(RawDamage));

	if (!this->DamageInfo.IsFixed) {
		float InstigatorCritPower = InstigatorComp->GetAttributeCurrentValue(EAttributeCode::ATT_CriticalPower);
		float InstigatorCritChance = InstigatorComp->GetAttributeCurrentValue(EAttributeCode::ATT_CriticalChance);
		float MovingValues = this->DamageInfo.MovingValues;
		float CritRand = FMath::RandRange(1, 100);
		bool isCritical = CritRand <= InstigatorCritChance;
		float DamageByMovement = RawDamage * (MovingValues / 100);
		float ElemDamageByMovement = RawElementalDamage * (MovingValues / 100);
		GLog->Log("DamageByMovement");
		GLog->Log(FString::SanitizeFloat(DamageByMovement));
		CalculatedPhysicalDamage = isCritical ? DamageByMovement + ( InstigatorCritPower * DamageByMovement ) : DamageByMovement;
		CalculatedElementalDamage = isCritical ? ElemDamageByMovement + (InstigatorCritPower * ElemDamageByMovement) : ElemDamageByMovement;
	}
	else {
		CalculatedPhysicalDamage = RawDamage;
		CalculatedElementalDamage = RawElementalDamage;
	}
	
	GLog->Log("CalculatedPhysicalDamage");
	GLog->Log(FString::SanitizeFloat(CalculatedPhysicalDamage));

	//sponge: might need to make a common func for defense calculation
	float TotalPhysicalDamage = CalculatedPhysicalDamage > 0.0 ? CalculatedPhysicalDamage * (CalculatedPhysicalDamage / (CalculatedPhysicalDamage + ReceiverDefense)) : 0.0;
	float TotalElementalDamage = CalculatedElementalDamage > 0.0 ? CalculatedElementalDamage * (CalculatedElementalDamage / (CalculatedElementalDamage + ReceiverElementalDefense)) : 0.0;
	
	//Shield Damage (elementals are not negated by shield)
	if (this->DamageInfo.IsAbsorbed && !this->DamageInfo.IsFixed) {
		TotalPhysicalDamage = TotalPhysicalDamage - (TotalPhysicalDamage * (ReceiverDamageAbsorption / 100));
	}
	float AccumulatedDamage = TotalPhysicalDamage + TotalElementalDamage;
	
	GLog->Log("Accumulated Damage");
	GLog->Log(FString::SanitizeFloat(AccumulatedDamage));


	if (this->DamageInfo.IsAbsorbed) {
		//Sponge: need to recheck numbers
		ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Stamina, ReceiverStamina - 10);
	}

	//sponge: might need amp def
	ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Health, ReceiverHealth - AccumulatedDamage);
	GLog->Log("ReceiverHealth");
	GLog->Log(FString::SanitizeFloat(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Health)));
}