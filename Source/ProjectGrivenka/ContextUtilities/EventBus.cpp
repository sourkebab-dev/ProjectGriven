// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBus.h"

void UEventBus::AddInstigatedEvtSubscriber(FInstigatedDelegate& OutDelegate, TEnumAsByte<EInstigatedDelegate> InEventCode)
{
	this->InstigatedObservers.Add(InEventCode, OutDelegate);
}

void UEventBus::AddSubscriber(FContextSimpleDelegate& OutDelegate, TEnumAsByte<EContextDelegates> InEventCode)
{
	this->Observers.Add(InEventCode, OutDelegate);
}

void UEventBus::AddEffectInitSubscriber(FRPGEffectInitDelegate& OutDelegate, TEnumAsByte<EEffectDelegates> InEffectInitCode)
{
	this->EffectApplyObservers.Add(InEffectInitCode, OutDelegate);
}
