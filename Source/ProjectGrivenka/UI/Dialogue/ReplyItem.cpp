// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplyItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ProjectGrivenka/Utilities/UIManager.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"

void UReplyItem::NativeConstruct()
{
	Super::NativeConstruct();
	this->ReplyButton->OnClicked.AddDynamic(this, &UReplyItem::OnReplyClick);
}

void UReplyItem::NativeDestruct()
{
	Super::NativeDestruct();
	this->ReplyButton->OnClicked.RemoveDynamic(this, &UReplyItem::OnReplyClick);
}

void UReplyItem::SetReplyData(FReplyData InReplyData)
{
	this->ReplyData = InReplyData;
	this->ReplyLabel->SetText(InReplyData.Text);
}

void UReplyItem::OnReplyClick()
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance || !GameInstance->UIManager) return;
	GameInstance->UIManager->EmitReplyDialogue(this->ReplyData.ReplyId);
}