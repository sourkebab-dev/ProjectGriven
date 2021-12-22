// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "ReplyBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UReplyBox : public UUserWidget
{
	GENERATED_BODY()
	
public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* ReplyBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UReplyItem> ReplyItemClass;

public:
	UFUNCTION(BlueprintCallable)
	virtual void DataSetup(TArray<FReplyData> ReplyData);
};
