// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemDefinitions.h"
#include "DialogueBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UDialogueBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UBorder* DialogueBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UReplyBox* ReplyBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	bool IsAQuestion = false;

public:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable)
	void SetDialogue(FDialogueData InDialogueData);

	UFUNCTION(BlueprintCallable)
	void SetIsAQuestion(bool InIsAQuestion);

	UFUNCTION(BlueprintCallable)
	void SetReplyData(TArray<FReplyData> InReplyData);
};
