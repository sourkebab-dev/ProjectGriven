// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "DeathState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FDeathMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DefaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HeavyMontage;
};

UCLASS(BlueprintType)
class PROJECTGRIVENKA_API UDeathState : public UBaseState
{
	GENERATED_BODY()
	
	FTimerHandle HitPauseTimer;

public:
	UPROPERTY(BlueprintReadWrite)
		UAnimMontage* CurrentDeathMontage;
	UPROPERTY(BlueprintReadWrite)
		AActor* DeathInstigator;
	UPROPERTY(BlueprintReadWrite)
		FDamageInfo DeathBlow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeathMontage DeathLeftMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeathMontage DeathRightMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeathMontage DeathFrontMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeathMontage DeathBackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeathMontage DeathUpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeathMontage DeathDownMontage;



public:
	virtual void Init_Implementation(class UCharacterStatesSystem* InStatesComp) override;
	UFUNCTION(BlueprintCallable)
	void OnReceiveDeathBlow(AActor* InDeathInstigator, FDamageInfo InDeathBlow);
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;


	UFUNCTION(BlueprintCallable)
	void StartDeathAnim();
};
