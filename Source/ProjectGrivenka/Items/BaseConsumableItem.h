// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BaseConsumableItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API ABaseConsumableItem : public ABaseItem
{
	GENERATED_BODY()
	
	virtual void OnUse() override;
};
