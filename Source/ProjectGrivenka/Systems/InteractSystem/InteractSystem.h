// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "InteractSystem.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractSystemStart, AActor*, InteractInstigator);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIVENKA_API UInteractSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* InteractableItem;

	UPROPERTY(BlueprintAssignable, Category = Events, meta = (DisplayName = "On Interact Start"))
	FInteractSystemStart OnInteract;

public:
	void Init() override;
	
	UFUNCTION()
	virtual void SaveInteractable(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void RemoveInteractable(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	AActor* GetInteractableItem();

	UFUNCTION(BlueprintCallable)
	void Interact(AActor* InteractInstigator);
};
