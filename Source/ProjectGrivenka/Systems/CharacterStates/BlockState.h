// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "BlockState.generated.h"

/**
 * 
 */

//Sponge: parry window is based on seconds, should probably be a frame?
static const float PARRYTIME = 0.2f;

UCLASS()
class PROJECTGRIVENKA_API UBlockState : public UBaseState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* HitInstigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo DamageInfo;

	FTimerHandle ParryTimer;
	bool IsParry = false;

public:
	UFUNCTION(BlueprintCallable)
	void OnReceiveHit(AActor* InHitInstigator, FDamageInfo InDamageInfo);
	void InvalidateParry();
	virtual bool StateValidation_Implementation() override;
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue) override;
	virtual void OnStateExit_Implementation() override;

};
