// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "BaseState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UBaseState : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer ActionTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTag;
	FGameplayTagContainer PrevActionTag;
	TEnumAsByte<EActionList> EnterAction;
	TEnumAsByte<EInputEvent> EnterEvent;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool StateValidation();
	virtual bool StateValidation_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(FCharacterContext InContext, class UCharacterStatesSystem* InStatesComp);
	virtual void Init_Implementation(FCharacterContext InContext, class UCharacterStatesSystem* InStatesComp);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionHandler(EActionList Action, EInputEvent EventType);
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AxisHandler(EActionList Action, float AxisValue);
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnStateEnter(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent);
	virtual void OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Tick(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnStateExit();
	virtual void OnStateExit_Implementation();
protected:
	FCharacterContext CharacterContext;
	class UCharacterStatesSystem* StatesComp;
};