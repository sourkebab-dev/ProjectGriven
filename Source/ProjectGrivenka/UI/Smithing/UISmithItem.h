// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UISmithItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUISmithItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* SmithButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SmithImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USmithingRecipePrefab* RecipePrefab;

public:
	UFUNCTION(BlueprintCallable)
	void RenderInfo();

	UFUNCTION(BlueprintCallable)
	void Disable();

	UFUNCTION(BlueprintCallable)
	void Enable();
};
