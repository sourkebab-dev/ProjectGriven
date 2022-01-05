// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ContextAvailable.generated.h"




USTRUCT(BlueprintType)
struct FCharacterContext
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPawnMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimInstance* CharacterAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AActor* CharacterActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UEventBus* EventBus;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UContextStore* Store;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UContextAvailable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTGRIVENKA_API IContextAvailable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetContext(FCharacterContext& OutContext);

};
