// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpawnPoint.h"
#include "kismet/GameplayStatics.h"
#include "ProjectGrivenka/Character/ControllableCharacter.h"

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

void APlayerSpawnPoint::SpawnPlayer_Implementation(FPersisted_CharacterCompleteData CharacterData)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AControllableCharacter* PlayerCharacter = this->GetWorld()->SpawnActor<AControllableCharacter>(CharacterData.Appearance.CharClass, this->GetActorLocation(), this->GetActorRotation(), SpawnInfo);
	if (!PlayerCharacter) {GLog->Log("Plauyer failed to spawn"); return;}
	//Sponge: Multi character spawn?
	PlayerCharacter->LoadData_Implementation(CharacterData);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(PlayerCharacter);
	GLog->Log("Spawned Player");
}


