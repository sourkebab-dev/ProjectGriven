// Fill out your copyright notice in the Description page of Project Settings.

#include "MapExit.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "BaseGameInstance.h"

// Sets default values
AMapExit::AMapExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMapExit::BeginPlay()
{
	Super::BeginPlay();	
}

void AMapExit::Interact_Implementation(AActor* InteractInstigator)
{
	if (this->MapExitType == EMapExitType::Normal) {
		UGameplayStatics::OpenLevel(this->GetWorld(), this->MapTargetName);
	}
	else if (this->MapExitType == EMapExitType::BountyContract) {
		UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
		UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();

		GameInstance->SetAcceptedBountyContract(AssetsData->BountyList->BountyList[0].BountyQuestID);
		FBountyContract AcceptedBounty = GameInstance->GetAcceptedBountyContract();
		if (AcceptedBounty.BountyQuestID.IsNone()) {
			GLog->Log("No Quest Selected");
			return;
		}
		//Sponge: maybe change to level object instead of level name? 
		UGameplayStatics::OpenLevel(this->GetWorld(), AcceptedBounty.LevelName);

	}
}

bool AMapExit::CheckIsInteractable_Implementation(AActor* InteractInstigator)
{
	return true;
}

