// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/CharacterSystem/BaseEffect.h"
#include "AttributeRegenOvertime.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UAttributeRegenOvertime : public UBaseEffect
{
	GENERATED_BODY()

public:
	virtual void InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FName InEffectId);
	virtual void OnExecuteEffect() override;
};
