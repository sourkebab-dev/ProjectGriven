// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseState.h"
#include "ProjectGrivenka/Interfaces/Interactable.h"
#include "InteractState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UInteractState : public UBaseState
{
	GENERATED_BODY()

	FInteractionInfo InteractionInfo;

public:	
	virtual bool StateValidation_Implementation() override;
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType) override;
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue) override;
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent) override;
	virtual void OnStateExit_Implementation() override;
	UFUNCTION()
	virtual void StartInteraction();
	UFUNCTION()
	virtual void EndInteraction();

};
