// Fill out your copyright notice in the Description page of Project Settings.


#include "UIInventoryContainer.h"
#include "UIItemPouch.h"

void UUIInventoryContainer::Render(AActor* Owner)
{
	this->ItemPouch->Render(Owner);
}
