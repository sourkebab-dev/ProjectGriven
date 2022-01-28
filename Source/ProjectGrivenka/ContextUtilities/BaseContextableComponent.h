// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseContextableComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UBaseContextableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UContextSystem* CompContext;

protected:
	UBaseContextableComponent();

public:	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Init();
	virtual void Init_Implementation();
};
