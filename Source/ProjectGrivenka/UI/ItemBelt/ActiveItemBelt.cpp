// Fill out your copyright notice in the Description page of Project Settings.


#include "ActiveItemBelt.h"
#include "Components/Image.h"


void UActiveItemBelt::SetItemInfo(FItemInfo ItemInfo)
{
	this->ActiveItemIcon->SetBrushFromTexture(ItemInfo.ItemIcon);
}
