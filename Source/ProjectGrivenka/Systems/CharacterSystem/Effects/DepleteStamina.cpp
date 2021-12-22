// Fill out your copyright notice in the Description page of Project Settings.


#include "DepleteStamina.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Systems/CharacterSystem/BaseEffect.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystem.h"

void UDepleteStamina::InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, float NewStaminaCost)
{
	this->StaminaCost = NewStaminaCost;
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef("Util_StaminaDepletion");
	if (!EffectPrefab) return;
	FEffectInfo TempEffectInfo = EffectPrefab->EffectInfo;
	TempEffectInfo.EffectProcesses[0].Value = NewStaminaCost;
	Super::Init(NewEffectInstigator, NewEffectReceiver, TempEffectInfo);
}

void UDepleteStamina::OnExecuteEffect()
{
	UCharacterSystem* EffectReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);
	EffectReceiverComp->Attributes->SetStamina(EffectReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Stamina) - this->StaminaCost);

	//Stamina depletion debounce
	EffectReceiverComp->RemoveEffectsByTag(FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Defaults.Activation.StaminaRegenBlocker"));
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef("Util_StaminaRegenBlocker");
	if (!EffectPrefab) return;
	FEffectInfo TempEffectInfo = EffectPrefab->EffectInfo;
	UBaseEffect* BlockStaminaRegen = NewObject<UBaseEffect>();
	BlockStaminaRegen->Init(this->EffectReceiver, this->EffectReceiver, TempEffectInfo);
	EffectReceiverComp->AddEffect(BlockStaminaRegen);
}