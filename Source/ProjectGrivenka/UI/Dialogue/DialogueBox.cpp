// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueBox.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBoxSlot.h"
#include "ProjectGrivenka/UI/Dialogue/ReplyBox.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"


FReply UDialogueBox::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance || !GameInstance->UIManager) return FReply::Unhandled();
	GameInstance->UIManager->EmitProceedDialogue();
	return FReply::Handled();
}

void UDialogueBox::SetDialogue(FDialogueData InDialogueData)
{
	this->Dialogue->SetText(InDialogueData.Text);
}

void UDialogueBox::SetIsAQuestion(bool InIsAQuestion)
{
	this->IsAQuestion = InIsAQuestion;
	UVerticalBoxSlot* ReplyBoxSlot = Cast<UVerticalBoxSlot>(this->ReplyBox->Slot);
	UVerticalBoxSlot* DialogueBoxSlot = Cast<UVerticalBoxSlot>(this->DialogueBox->Slot);
	if (!ReplyBoxSlot || !DialogueBoxSlot) { GLog->Log("Dialogue box slots invalid"); return; }

	FSlateChildSize ChildSize;
	ChildSize.SizeRule = ESlateSizeRule::Fill;
	ChildSize.Value = 1.0f;
	DialogueBoxSlot->SetSize(ChildSize);

	if (InIsAQuestion) {
		ReplyBoxSlot->SetSize(ChildSize);
	}
	else {
		FSlateChildSize ReplyMinimize;
		ReplyMinimize.SizeRule = ESlateSizeRule::Fill;
		ReplyMinimize.Value = 0.0f;
		ReplyBoxSlot->SetSize(ReplyMinimize);
	}
}

void UDialogueBox::SetReplyData(TArray<FReplyData> InReplyData)
{
	this->ReplyBox->DataSetup(InReplyData);
}
