// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UENUM(BlueprintType)
enum EInteractablesLocomotionType
{
	INL_BLOCK UMETA(DisplayName = "Stop Movement"),
	INL_SLOW       UMETA(DisplayName = "Slow Down Movement"),
	INL_NORMAL UMETA(DisplayName = "Normal Movement"),
};

UENUM(BlueprintType)
enum EInteractablesUsageType
{
	INU_ONEOFF UMETA(DisplayName = "One Off Interactable Usage"),
	INU_TOGGLE       UMETA(DisplayName = "Toggle Interactable Usage"),
	INU_HOLD UMETA(DisplayName = "Hold Interactable Usage"),
};

USTRUCT(BlueprintType)
struct FInteractionInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLatent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LatentForceExitTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UAnimMontage* InteractionAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EInteractablesLocomotionType> LocomotionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EInteractablesUsageType> UsageType;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UInteractSystem* GetInteractSystemComp();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetInteractableItem();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckIsInteractable(AActor* InteractionInstigator);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RequestInteractionInfo(AActor* InteractionInstigator, FInteractionInfo& OutInteractionInfo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UBoxComponent* GetInteractionCollider();
};
