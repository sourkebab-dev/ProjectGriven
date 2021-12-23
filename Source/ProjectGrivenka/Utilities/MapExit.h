// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectGrivenka/Interfaces/Interactable.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "MapExit.generated.h"

UCLASS()
class PROJECTGRIVENKA_API AMapExit : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapExit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapInfo")
	FName MapTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapInfo")
	TEnumAsByte<EMapExitType> MapExitType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void Interact_Implementation(AActor* InteractInstigator);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CheckIsInteractable(AActor* InteractInstigator);
	virtual bool CheckIsInteractable_Implementation(AActor* InteractInstigator) override;

};
