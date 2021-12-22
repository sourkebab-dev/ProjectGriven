// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeRegenOvertime.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystem.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

void UAttributeRegenOvertime::InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FName InEffectId)
{
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef(InEffectId);
	if (!EffectPrefab) return;
	FEffectInfo TempEffectInfo = EffectPrefab->EffectInfo;
	Super::Init(NewEffectInstigator, NewEffectReceiver, TempEffectInfo);
}

void UAttributeRegenOvertime::OnExecuteEffect()
{
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	switch (this->EffectInfo.EffectProcesses[0].AffectedAttribute)
	{
		case EAttributeCode::ATT_Health:
			ReceiverComp->Attributes->SetHealth(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Health) + ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_HealthRecoverRate));
			break;
		case EAttributeCode::ATT_Stamina:
			ReceiverComp->Attributes->SetStamina(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Stamina) + ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_StaminaRecoverRate));
			break;
		case EAttributeCode::ATT_Amp:
			ReceiverComp->Attributes->SetAmp(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Amp) - ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_AmpRecoverRate));
			break;
		case EAttributeCode::ATT_Fortitude:
			ReceiverComp->Attributes->SetFortitude(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Fortitude) + ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_FortitudeRecoverRate));
			break;
		default:
			break;
	}
}

