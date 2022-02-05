// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIInventoryContainer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUIInventoryContainer : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUIItemPouch* ItemPouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OwnerInventory;

public: 
	UFUNCTION(BlueprintCallable)
	void Render(AActor* Owner);

};
