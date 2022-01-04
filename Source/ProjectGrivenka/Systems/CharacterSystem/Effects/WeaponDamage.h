// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/CharacterSystem/BaseEffect.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "WeaponDamage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UWeaponDamage : public UBaseEffect
{
	GENERATED_BODY()

	FAttackValues AttackValue;

public:
	void InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FAttackValues InAttackValue);
	virtual void OnExecuteEffect() override;
};
