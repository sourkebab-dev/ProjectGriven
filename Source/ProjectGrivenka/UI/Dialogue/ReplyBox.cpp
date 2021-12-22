// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplyBox.h"
#include "Components/ScrollBox.h"
#include "Blueprint/WidgetTree.h"
#include "ReplyItem.h"

void UReplyBox::DataSetup(TArray<FReplyData> ReplyData)
{
	this->ReplyBox->ClearChildren();
	for (int i = 0; i < ReplyData.Num(); i++) {
		UReplyItem* TempItem = this->WidgetTree->ConstructWidget<UReplyItem>(this->ReplyItemClass);
		TempItem->SetReplyData(ReplyData[i]);
		this->ReplyBox->AddChild(TempItem);
	}
}

