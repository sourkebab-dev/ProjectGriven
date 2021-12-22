// Fill out your copyright notice in the Description page of Project Settings.


#include "LootSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

TArray<FLootItem> ULootSystem::GenerateLoot()
{
	TArray<FLootItem> GeneratedLoot;
	float SumOfWeight = 0;
	for (int i = 0; i < this->DynamicLootItems.Num(); i++) {
		SumOfWeight += this->DynamicLootItems[i].LootWeight;
	}
	for (int i = 0; i < 3; i++) {
		float PooledSearch = 0;
		float RandValue = FMath::RandRange(0.0f, SumOfWeight);
		
		for (int j = 0; j < this->DynamicLootItems.Num(); j++) {
			PooledSearch += this->DynamicLootItems[j].LootWeight;
			if (RandValue <= PooledSearch) {
				GeneratedLoot.Add(this->DynamicLootItems[j]);
				//Sponge: Might need to change once taken to an int
				if (this->DynamicLootItems[j].OnceTaken) {
					SumOfWeight -= this->DynamicLootItems[j].LootWeight;
					this->DynamicLootItems.RemoveAt(j);
				}
				break;
			}
		}
	}

	GeneratedLoot.Append(this->StaticLootItems);
	return GeneratedLoot;
}

void ULootSystem::ReceiveAction(FGameplayTag InAction)
{
	//Sponge: Might need to recheck
	for (int i = 0; i < this->GrantableLoot.Num() ; i++) {
		if (!this->GrantableLoot[i].ActionTag.MatchesTagExact(InAction)) continue;
		
		if (this->GrantableLoot[i].AlwaysGiven) {
			this->DynamicLootItems.Add(this->GrantableLoot[i].LootGiven);
		}
		else {
			this->StaticLootItems.Add(this->GrantableLoot[i].LootGiven);
		}

		this->GrantableLoot.RemoveAt(i);
		i--;
	}
}

void ULootSystem::DebugBullshit()
{
	GLog->Log("Bullshit");
}
