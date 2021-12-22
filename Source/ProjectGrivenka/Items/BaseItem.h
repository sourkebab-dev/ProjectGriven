// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDefinitions.h"
#include "BaseItem.generated.h"

UCLASS()
class PROJECTGRIVENKA_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemInfo ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* ItemMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	ABaseItem();

	virtual void OnUse();
	virtual void LoadInfo(FItemInfo InItemInfo);
};
