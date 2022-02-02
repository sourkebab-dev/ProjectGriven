// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UISmithTree.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UUISmithTree : public UUserWidget
{
	GENERATED_BODY()
	
public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USmithingRecipeTreePrefab* RecipeTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<class USmithingRecipePrefab*, class UUISmithItem*> RecipeToUIMap;

public:
	UFUNCTION(BlueprintCallable)
	void Render(enum EEquipmentType InEquipmentType, enum EEquipmentTree InChosenEquipment, FGuid InEquipmentInsGuid, FName InEquipmentId);

};
