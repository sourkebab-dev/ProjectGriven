// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "LootSystemDefinitions.h"
#include "LootSystem.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API ULootSystem : public UBaseContextableComponent
{
	GENERATED_BODY()


public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLootItem> DynamicLootItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLootItem> StaticLootItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FActionTagToLoot> GrantableLoot;

public:
	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GenerateLoot();

	UFUNCTION(BlueprintCallable)
	void ReceiveAction(FGameplayTag InAction);

	UFUNCTION(BlueprintCallable)
	void DebugBullshit();

};
