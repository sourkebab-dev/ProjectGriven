// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ControlSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIVENKA_API UControlSystem : public UBaseContextableComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RawInput;

protected:
	void ControlCycleItem(float Value);
	void ControlMoveForward(float Value);
	void ControlMoveRight(float Value);
	void ControlAttack();
	void ControlAttackRelease();
	void ControlToggleAmpField();
	void ControlUseItem();
	void ControlVentAmp();
	void ControlDodge();
	void ControlInteract();
	void UpdateWorldSpaceVectors();

public:
	void Init() override;
	UFUNCTION()
	void ControlSystemSetup(AController* NewController);
	UFUNCTION()
	void ControlSystemDisable(AController* OldController);
	UFUNCTION()
	void ControlSystemPossess(AActor* PossessInstigator);

};
