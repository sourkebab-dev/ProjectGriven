// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpawnPoint.h"
#include "kismet/GameplayStatics.h"
#include "ProjectGrivenka/Character/ControllableCharacter.h"
#include "ProjectGrivenka/PersistedData/Persistable.h"

// Sets default values
APlayerSpawnPoint::APlayerSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APlayerSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerSpawnPoint::SpawnPlayer_Implementation(FPersistedCharacterData CharacterData)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* PlayerCharacter = this->GetWorld()->SpawnActor<AActor>(CharacterData.Appearance.CharClass, this->GetActorLocation(), this->GetActorRotation(), SpawnInfo);
	if (!PlayerCharacter || !PlayerCharacter->Implements<UPersistable>()) {GLog->Log("Player failed to spawn"); return;}
	IPersistable::Execute_LoadPersistance(PlayerCharacter);
	//UGameplayStatics::GetPlayerController(GetWorld(), 0)->UnPossess();
	//UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(PlayerCharacter);
	GLog->Log("Spawned Player");
}


