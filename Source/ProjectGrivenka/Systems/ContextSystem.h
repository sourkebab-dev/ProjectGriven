// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ContextSystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FContextCombat
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttackValues CurrentAttack;
};


USTRUCT(BlueprintType)
struct FContextMovement
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector WorldSpaceTargetDir;
};

USTRUCT(BlueprintType)
struct FContextInfo
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGuid InstanceGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName InstanceName;
};


UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UContextSystem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPawnMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimInstance* CharacterAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AController* Controller;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AActor* CharacterActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UEventBus* EventBus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FContextCombat CombatModule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FContextMovement MovementModule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FContextInfo InfoModule;

};
