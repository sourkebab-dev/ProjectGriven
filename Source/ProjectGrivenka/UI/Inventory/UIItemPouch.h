// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIItemPouch.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUIItemPouch : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UUniformGridPanel* PouchGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUIItemPouchSlot> ItemPouchSlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ColumnNumber = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SlotNumber = 30;


public:
	UFUNCTION(BlueprintCallable)
	void Render(AActor* Owner);

};
