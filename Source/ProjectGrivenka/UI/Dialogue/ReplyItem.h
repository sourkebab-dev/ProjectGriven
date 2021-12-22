// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "ReplyItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UReplyItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ReplyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ReplyLabel;
	
	FReplyData ReplyData;

public:
	UFUNCTION(BlueprintCallable)
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void SetReplyData(FReplyData InReplyData);

	UFUNCTION(BlueprintCallable)
	void OnReplyClick();

};
