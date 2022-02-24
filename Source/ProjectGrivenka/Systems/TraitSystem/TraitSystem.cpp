// Fill out your copyright notice in the Description page of Project Settings.


#include "TraitSystem.h"
#include "TraitSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/EffectPrefab.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesAvailable.h"
#include "ProjectGrivenka/Systems/TraitSystem/TraitPrefab.h"
#include "ProjectGrivenka/Systems/TraitSystem/AbilityPrefab.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"

void UTraitSystem::LoadTraits(FPersistedTraitData InTraitData)
{
	this->TraitIds = InTraitData.TraitIds;
	for (int i = 0; i < InTraitData.TraitIds.Num(); i++) {
		this->ActivateTrait(InTraitData.TraitIds[i]);
	}
}

void UTraitSystem::AddTrait(FName InTraitId)
{
	this->TraitIds.Add(InTraitId);
	this->ActivateTrait(InTraitId);
}

void UTraitSystem::ActivateTrait(FName InTraitId)
{
	auto GrivenkaData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	auto TraitPrefab = GrivenkaData->TraitPrefab->TraitList.FindRef(InTraitId);
	GLog->Log("ActovateTrait");
	if (!TraitPrefab) return;
	GLog->Log("PrefabFound");

	if (this->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		for (int i = 0; i < TraitPrefab->TraitInfo.AttributeTraits.Num(); i++) {
			GLog->Log("InitEffect");
			ICharacterSystemAvailable::Execute_InitEffectByPrefab(this->CompContext->CharacterActor, nullptr, TraitPrefab->TraitInfo.AttributeTraits[i], 0.0f, false);
		}
	}

	if (this->CompContext->CharacterActor->Implements<UCharacterStatesAvailable>()) {
		for (int i = 0; i < TraitPrefab->TraitInfo.AbilityTraits.Num(); i++) {
			ICharacterStatesAvailable::Execute_GrantAction(this->CompContext->CharacterActor, TraitPrefab->TraitInfo.AbilityTraits[i]->AbilityInfo.GrantedAbility);
		}
	}
	

}

void UTraitSystem::DisableTrait(FName InTraitId)
{
	auto GrivenkaData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	auto TraitPrefab = GrivenkaData->TraitPrefab->TraitList.FindRef(InTraitId);
	if (!TraitPrefab) return;

	if (this->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		for (int i = 0; i < TraitPrefab->TraitInfo.AttributeTraits.Num(); i++) {
			ICharacterSystemAvailable::Execute_RemoveEffectById(this->CompContext->CharacterActor, TraitPrefab->TraitInfo.AttributeTraits[i]->EffectInfo.EffectId);
		}
	}

	if (this->CompContext->CharacterActor->Implements<UCharacterStatesAvailable>()) {
		for (int i = 0; i < TraitPrefab->TraitInfo.AbilityTraits.Num(); i++) {
			ICharacterStatesAvailable::Execute_RemoveAction(this->CompContext->CharacterActor, TraitPrefab->TraitInfo.AbilityTraits[i]->AbilityInfo.GrantedAbility);
		}
	}
}

void UTraitSystem::RemoveTrait(FName InTraitId)
{
	this->TraitIds.Remove(InTraitId);
	this->DisableTrait(InTraitId);
}
