// Fill out your copyright notice in the Description page of Project Settings.


#include "GrivenkaSingletonLibrary.h"

UGrivenkaDataSingleton* UGrivenkaSingletonLibrary::GetGrivenkaData()
{
	UGrivenkaDataSingleton* DataInstance = Cast<UGrivenkaDataSingleton>(GEngine->GameSingleton);
	if (!DataInstance || !DataInstance->IsValidLowLevel()) return NULL;
	return DataInstance;
}
