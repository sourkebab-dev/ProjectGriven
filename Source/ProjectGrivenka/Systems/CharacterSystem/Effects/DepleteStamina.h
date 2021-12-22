// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/CharacterSystem/BaseEffect.h"
#include "DepleteStamina.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UDepleteStamina : public UBaseEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	float StaminaCost;

public:
	UFUNCTION(BlueprintCallable)
	virtual void InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, float NewStaminaCost);
	virtual void OnExecuteEffect() override;

};
