// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "UIStaminaBar.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUIStaminaBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	AActor* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* Bar;

	FSimpleDynamicDelegate OnRender;

public:
	UFUNCTION(BlueprintCallable)
	void Setup(AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	void Render();

	UFUNCTION(BlueprintCallable)
	void OnRemove();
};
