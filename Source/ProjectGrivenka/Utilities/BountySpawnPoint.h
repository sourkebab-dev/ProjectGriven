// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "BountySpawnPoint.generated.h"

UCLASS()
class PROJECTGRIVENKA_API ABountySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABountySpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SpawnBounty(FPersisted_CharacterCompleteData CharacterData);
	virtual void SpawnBounty_Implementation(FPersisted_CharacterCompleteData CharacterData);

};
