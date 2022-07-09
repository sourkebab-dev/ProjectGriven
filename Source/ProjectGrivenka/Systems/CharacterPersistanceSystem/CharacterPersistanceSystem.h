// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/PersistedData/Persistable.h"
#include "CharacterPersistanceSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIVENKA_API UCharacterPersistanceSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGenerated = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPersistedCharacterData CharacterInitializer;

public:
	void Init_Implementation() override;

	void GenerateWearables();

	void GenerateBody();

	FName GetRandomEquipmentVariants(TArray<FEquipmentChance> InEquipmentPack);

	UFUNCTION(BlueprintImplementableEvent)
	FBodyMorphPacks GetBodyMorphPacks();

	UFUNCTION(BlueprintImplementableEvent)
	FEquipmentPacks GetEquipmentPacks();

	UFUNCTION(BlueprintCallable)
	void GenerateCharacter();

	UFUNCTION(BlueprintCallable)
	void LoadData(FPersistedCharacterData InCharacterData);

	UFUNCTION(BlueprintCallable)
	void SaveData(FPersistedCharacterData& OutCharacterData);
};
