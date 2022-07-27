// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "PlayerSpawnPoint.generated.h"

UCLASS()
class PROJECTGRIVENKA_API APlayerSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* SpawnPlayer(FPersistedCharacterData CharacterData, bool IsSpawnedAsNpc = false);
	virtual AActor* SpawnPlayer_Implementation(FPersistedCharacterData CharacterData, bool IsSpawnedAsAI = false);

};
