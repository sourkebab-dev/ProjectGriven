// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/CharacterSystem/BaseEffect.h"
#include "WeaponDamage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UWeaponDamage : public UBaseEffect
{
	GENERATED_BODY()

public:
	virtual void Init(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FEffectInfo InEffectInfo) override;
	virtual void OnExecuteEffect() override;
};
