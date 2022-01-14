// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "CharacterSystemDefinitions.h"
#include "BaseEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UBaseEffect : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PooledDuration = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle DurationHandler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle TickHandler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* EffectInstigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* EffectReceiver;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Info")
	FEffectInfo EffectInfo;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Init(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FEffectInfo InEffectInfo);
	UFUNCTION(BlueprintCallable)
	virtual void OnActivated();
	UFUNCTION(BlueprintCallable)
	virtual void OnForceInterrupt();
	UFUNCTION(BlueprintCallable)
	virtual void OnDeactivated();
	UFUNCTION(BlueprintCallable)
	virtual void OnDurationTick();
	UFUNCTION(BlueprintCallable)
	virtual void OnPreExecuteEffect(); //h
	UFUNCTION(BlueprintCallable)
	virtual void OnExecuteEffect(); //main effect logic
	UFUNCTION(BlueprintCallable)
	virtual void OnPostExecuteEffect(); //cycle to handle an "after" effect
	UFUNCTION(BlueprintCallable)
	virtual void OnEnded(); // cycle to remove handlers like timers, delegates etc
	UFUNCTION(BlueprintCallable)
	void RemoveSelf();

protected:
	UFUNCTION(BlueprintCallable)
	virtual void BasicSumExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue);
	UFUNCTION(BlueprintCallable)
	virtual void BasicMultiplyExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue);
	UFUNCTION(BlueprintCallable)
	virtual void BasicOverwriteExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue);
	UFUNCTION(BlueprintCallable)
	virtual void RevertSumExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue);
	UFUNCTION(BlueprintCallable)
	virtual void RevertMultiplyExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue);
	UFUNCTION(BlueprintCallable)
	virtual void RevertOverwriteExecute(TEnumAsByte<EAttributeCode> InAttributeCode, float InValue);

};
