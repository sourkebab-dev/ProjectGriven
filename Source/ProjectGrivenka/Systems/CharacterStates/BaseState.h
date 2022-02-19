// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
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
	class UCharacterStatesSystem* StatesComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer ActionTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPersistant;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool StateValidation();
	virtual bool StateValidation_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init(class UCharacterStatesSystem* InStatesComp);
	virtual void Init_Implementation(class UCharacterStatesSystem* InStatesComp);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionHandler(EActionList Action, EInputEvent EventType);
	virtual void ActionHandler_Implementation(EActionList Action, EInputEvent EventType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AxisHandler(EActionList Action, float AxisValue);
	virtual void AxisHandler_Implementation(EActionList Action, float AxisValue);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnStateEnter();
	virtual void OnStateEnter_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Tick(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnStateExit();
	virtual void OnStateExit_Implementation();
protected:
};
