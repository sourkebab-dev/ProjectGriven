// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "LatentAction.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API ULatentAction : public UAnimNotifyState
{
	GENERATED_BODY()
	
	void NotifyBegin(class USkeletalMeshComponent* Mesh, class UAnimSequenceBase* Animation, float TotalDuration) override;
};
