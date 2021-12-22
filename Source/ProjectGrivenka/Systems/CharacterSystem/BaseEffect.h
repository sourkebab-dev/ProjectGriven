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
	//Force delete effect without turning off
	UFUNCTION(BlueprintCallable)
	virtual void OnForceInterrupt();
	//Turning off without deleting effects
	UFUNCTION(BlueprintCallable)
	virtual void OnDeactivated();
	UFUNCTION(BlueprintCallable)
	virtual void OnDurationTick();
	UFUNCTION(BlueprintCallable)
	virtual void OnTick();
	UFUNCTION(BlueprintCallable)
	virtual void OnPreExecuteEffect();
	UFUNCTION(BlueprintCallable)
	virtual void OnExecuteEffect();
	//turn off process: post & onend
	UFUNCTION(BlueprintCallable)
	virtual void OnPostExecuteEffect();
	UFUNCTION(BlueprintCallable)
	virtual void OnEnded();
	UFUNCTION(BlueprintCallable)
	void RemoveSelf();
	UFUNCTION(BlueprintCallable)
	void UpdateEffectInfo(FEffectInfo InEffectInfo);
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
