// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "DialogueSystemDefinitions.h"
#include "DialogueSystem.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UDialogueSystem : public UBaseContextableComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* DefaultDialogueTree;

protected:

public:	
	void Init() override;

	UFUNCTION(BlueprintCallable)
	void StartDefaultDialogue();
	
	UFUNCTION(BlueprintCallable)
	void StopDialogue();
};
