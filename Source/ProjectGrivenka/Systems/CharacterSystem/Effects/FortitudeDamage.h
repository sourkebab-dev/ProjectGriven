// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Systems/CharacterSystem/BaseEffect.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "FortitudeDamage.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UFortitudeDamage : public UBaseEffect
{
	GENERATED_BODY()
	
	FDamageInfo DamageInfo;

public:
	void InitOverloaded(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FDamageInfo InDamageInfo);
	virtual void OnExecuteEffect() override;
	virtual void OnPostExecuteEffect() override;

};
