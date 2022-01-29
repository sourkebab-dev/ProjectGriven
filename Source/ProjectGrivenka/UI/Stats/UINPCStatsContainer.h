// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UINPCStatsContainer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUINPCStatsContainer : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
		AActor* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UUIHealthBar* Healthbar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UUIStaminaBar* StaminaBar;

public:
	UFUNCTION(BlueprintCallable)
	void Setup(AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	void Remove();

};
