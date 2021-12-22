// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "ProjectGrivenka/PersistedData/Persistable.h"
#include "BaseEquipment.generated.h"

UCLASS()
class PROJECTGRIVENKA_API ABaseEquipment : public AActor, public IPersistable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEquipment();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EquipmentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* EquipmentIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* EquipmentMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SaveData();
	virtual void SaveData_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadData(FPersisted_CharacterCompleteData CharacterData);
	virtual void LoadData_Implementation(FPersisted_CharacterCompleteData CharacterData);


	FORCEINLINE class USkeletalMeshComponent* GetEquipmentMesh() const { return EquipmentMesh; }

};
