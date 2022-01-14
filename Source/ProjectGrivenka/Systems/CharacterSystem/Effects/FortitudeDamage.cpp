// Fill out your copyright notice in the Description page of Project Settings.


#include "FortitudeDamage.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

void UFortitudeDamage::InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FDamageInfo InDamageInfo) {
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef("Util_FortitudeDamage");
	if (!EffectPrefab) return;
	FEffectInfo TempEffectInfo = EffectPrefab->EffectInfo;
	this->DamageInfo = InDamageInfo;
	Super::Init(NewEffectInstigator, NewEffectReceiver, TempEffectInfo);
}

void UFortitudeDamage::OnExecuteEffect()
{
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	float CurrentFortitude = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Fortitude);
	float CurrentMaxFortitude = ReceiverComp->GetAttributeMaxValue(EAttributeCode::ATT_Fortitude);
	if (CurrentFortitude <= 0) {
		ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Fortitude, CurrentMaxFortitude);
		CurrentFortitude = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Fortitude);
	}

	float FortitudeDamage = 0.0;
	switch (this->DamageInfo.ImpactType) {
	case EDamageImpactType::DI_HIGH:
		FortitudeDamage = 24;
		break;
	case EDamageImpactType::DI_MEDIUM:
		FortitudeDamage = 16;
		break;
	case EDamageImpactType::DI_LOW:
		FortitudeDamage = 8;
		break;
	}
	
	ReceiverComp->SetAttributeValue( EAttributeCode::ATT_Fortitude, CurrentFortitude - FortitudeDamage);
	GLog->Log("CurrFortitude");
	GLog->Log(FString::SanitizeFloat(CurrentFortitude - FortitudeDamage));
}

void UFortitudeDamage::OnPostExecuteEffect()
{
	//Restore fortitude to full after fortitude reset timer finished (config is in prefab)
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	float MaxFortitude = ReceiverComp->GetAttributeMaxValue(EAttributeCode::ATT_Fortitude);
	ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Fortitude, MaxFortitude);
}