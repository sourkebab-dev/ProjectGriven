// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Items/ItemDefinitions.h"
#include "ActiveItemBelt.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UActiveItemBelt : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* ActiveItemIcon;

public:
	UFUNCTION(BlueprintCallable)
	void SetItemInfo(FItemInfo ItemInfo);
};
