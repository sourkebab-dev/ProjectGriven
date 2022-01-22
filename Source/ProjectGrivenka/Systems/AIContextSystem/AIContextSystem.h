// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "AIContextSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTGRIVENKA_API UAIContextSystem : public UBaseContextableComponent
{
	GENERATED_BODY()



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EHostilityType> HostilityType = EHostilityType::NEUTRAL;

public:
	void Init() override;

	UFUNCTION(BlueprintCallable)
	void SignalCommandToActor(AActor* CommandReceiveActor, FCommandInfo CommandInfo);

	UFUNCTION(BlueprintCallable)
	void SignalCommandToArea(FCommandInfo CommandInfo);

	UFUNCTION(BlueprintCallable)
	void OnCommanded(AActor* CommandInstigator, FCommandInfo CommandInfo);

	UFUNCTION(BlueprintCallable)
	bool CheckHostility(AActor* ToCheckActor);	
};
