// Fill out your copyright notice in the Description page of Project Settings.

#include "BountySpawnPoint.h"
#include "kismet/GameplayStatics.h"
#include "ProjectGrivenka/Character/BaseCharacter.h"

// Sets default values
ABountySpawnPoint::ABountySpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABountySpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABountySpawnPoint::SpawnBounty_Implementation(FPersisted_CharacterCompleteData CharacterData)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABaseCharacter* Bounty = this->GetWorld()->SpawnActor<ABaseCharacter>(CharacterData.Appearance.CharClass ,this->GetActorLocation(), this->GetActorRotation(), SpawnInfo);
	if (!Bounty) { GLog->Log("bounte failed to spawn"); return; }

	//Sponge: Multi character spawn?
	Bounty->LoadData_Implementation(CharacterData);
}



