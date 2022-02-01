// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "ProjectGrivenka/Items/ItemDefinitions.h"
#include "ProjectGrivenka/Quest/BountyList.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = "true"))
	FName AcceptedBountyContractId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player & Crew", meta = (AllowPrivateAccess = "true"))
	FGuid ControlledCrewId;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player & Crew")
	TArray<AActor*> PartyInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player & Crew")
	TArray<FPersistedCharacterData> Crew;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FEquipmentBoxItem> EquipmentBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FPersistedInventoryItems> ItemBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FPersistedInventoryItems> MaterialBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UUIManager* UIManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUIManager> UIManagerClass;

public:
	void Init() override;
	UFUNCTION(BlueprintCallable)
	void SetControlledCrewId(FGuid InGuid) { this->ControlledCrewId = InGuid; }
	UFUNCTION(BlueprintCallable)
	FGuid GetControlledCrewId() { return this->ControlledCrewId; }
	UFUNCTION(BlueprintCallable)
	void SetAcceptedBountyContract(FName BountyContractID) { this->AcceptedBountyContractId = BountyContractID;  }
	UFUNCTION(BlueprintCallable)
	FName GetAcceptedBountyContractId() { return this->AcceptedBountyContractId;  }
	UFUNCTION(BlueprintCallable)
	FBountyContract GetAcceptedBountyContract();
	UFUNCTION(BlueprintCallable)
	void StoreMaterial(FItemInfo MaterialInfo, int Count);
	UFUNCTION(BlueprintCallable)
	void DepleteMaterial(FItemInfo MaterialInfo, int Count);
	UFUNCTION(BlueprintCallable)
	bool CheckSmithingRequirements(FItemInfo MaterialInfo, int Count);
};
