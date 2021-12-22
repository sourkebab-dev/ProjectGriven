// Fill out your copyright notice in the Description page of Project Settings.


#include "LatentAction.h"
#include "ProjectGrivenka/Character/BaseCharacter.h"

void ULatentAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration)
{
	ABaseCharacter* CharInstance = Cast<ABaseCharacter>(Mesh->GetOwner());
	if (CharInstance) {
		CharInstance->LatentActionDelegate.Broadcast();
	}
}