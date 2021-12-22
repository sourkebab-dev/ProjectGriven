// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "BaseContextableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UBaseContextableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterContext CompContext;

protected:
	UBaseContextableComponent();

public:	
	UFUNCTION(BlueprintCallable)
	virtual void Init();
};
