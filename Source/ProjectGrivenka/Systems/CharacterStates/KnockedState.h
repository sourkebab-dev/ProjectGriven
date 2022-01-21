// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "KnockedState.generated.h"

/**
 * 
 */

static const float DOTDIRECTIONTRESHOLD = 0.8;

USTRUCT(BlueprintType)
struct FStunMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DefaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HeavyMontage;
};

UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UKnockedState : public UBaseState
{
	GENERATED_BODY()


	FTimerHandle HitPauseTimer;

	FVector PushStartLocation;
	FVector PushTargetLocation;
	float PooledTime = 0.0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsStaggeredOnEmptyFortitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalPushTime = 0.25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PushDistanceMultiplier = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CurrentStunMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* HitInstigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo DamageInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStunMontage StunLeftMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStunMontage StunRightMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStunMontage StunFrontMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStunMontage StunBackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStunMontage StunUpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FStunMontage StunDownMontage;

public:
	virtual void Init_Implementation(class UCharacterStatesSystem* InStatesComp) override;
	virtual void Tick_Implementation(float DeltaTime);
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void OnStateExit_Implementation() override;
	

	UFUNCTION(BlueprintCallable)
	void OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo);
	UFUNCTION(BlueprintCallable)
	void StartHitReact();
	UFUNCTION(BlueprintCallable)
	void OnHitReactEnd(UAnimMontage* Montage, bool bInterrupted);

};
