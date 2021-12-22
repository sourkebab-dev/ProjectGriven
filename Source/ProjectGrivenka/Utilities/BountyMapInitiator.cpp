// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyMapInitiator.h"
#include "BountySpawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawnPoint.h"
#include "BaseGameInstance.h"

// Sets default values
ABountyMapInitiator::ABountyMapInitiator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABountyMapInitiator::BeginPlay()
{
	Super::BeginPlay();
	GLog->Log("Loaded");
	TArray<AActor*> PlayerSpawnPoints;
	TArray<AActor*> BountySpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), APlayerSpawnPoint::StaticClass(), PlayerSpawnPoints);
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), ABountySpawnPoint::StaticClass(), BountySpawnPoints);
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	FBountyContract AcceptedBounty = GameInstance->GetAcceptedBountyContract();

	if (AcceptedBounty.BountyQuestID.IsNone()) { GLog->Log("No Contract Accepted"); return; }
	if (!PlayerSpawnPoints.Num()) { GLog->Log("No Player Spawn Point in this level"); return; }
	if (!BountySpawnPoints.Num()) { GLog->Log("No Bounty Spawn Point in this level"); return; }


	for (int i = 0; i < AcceptedBounty.BountyTargets.Num(); i++) {
		int idxSpawnLoc = FMath::RandRange(0, BountySpawnPoints.Num() - 1);
		ABountySpawnPoint* SelectedBountySpawnPoint = Cast<ABountySpawnPoint>(BountySpawnPoints[idxSpawnLoc]);
		SelectedBountySpawnPoint->SpawnBounty_Implementation(AcceptedBounty.BountyTargets[i]->CharacterData);
	}

	APlayerSpawnPoint* PlayerSpawnPoint = Cast<APlayerSpawnPoint>(PlayerSpawnPoints[0]);
	//SPONGE: uses crew 1 as a placeholder, need to remove later once crew system is fully persisted
	FPersisted_CharacterCompleteData CurrentChar = GameInstance->Crew[1];
	for (int i = 0; i < GameInstance->Crew.Num(); i++) {
		if (GameInstance->GetControlledCrewId() == GameInstance->Crew[i].Info.CharacterId) {
			CurrentChar = GameInstance->Crew[i];
			GLog->Log("test equipment peristance on spawn");
			GLog->Log(GameInstance->Crew[i].Equipments.WeaponInfo.BaseClass->GetFullName());
		}
	}

	PlayerSpawnPoint->SpawnPlayer_Implementation(CurrentChar);

}

