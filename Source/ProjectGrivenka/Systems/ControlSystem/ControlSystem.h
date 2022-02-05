// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ControlSystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTGRIVENKA_API UControlSystem : public UBaseContextableComponent
{
	GENERATED_BODY()

	UControlSystem();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraMain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RawInput;
	UPROPERTY(BlueprintReadWrite)
	AActor* CommandedActor;
	UPROPERTY(BlueprintReadWrite)
	class UBaseGameInstance* GI;
	UPROPERTY(BlueprintReadWrite)
	AActor* LockedActor;

	float RotationRate = 20;

protected:
	void ControlCycleItem(float Value);
	void ControlMouseX(float Value);
	void ControlMouseY(float Value);
	void ControlMoveForward(float Value);
	void ControlMoveRight(float Value);
	void ControlAttack();
	void ControlAttackRelease();
	void ControlBlock();
	void ControlBlockRelease();
	void ControlToggleAmpField();
	void ControlUseItem();
	void ControlVentAmp();
	void ControlDodge();
	void ControlInteract();
	void ControlInventory();
	void ControlLockOn();
	void ControlCommand1();
	void ControlCommand2();
	void ControlCommand3();
	void ControlCommandCancel();
	void UpdateWorldSpaceVectors();


public:
	void Init_Implementation() override;
	UFUNCTION()
	void ControlSystemSetup(AController* NewController);
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	UFUNCTION()
	void AnimHandler(EAnimEvt InAnimEvt);
	UFUNCTION()
	void ControlSystemDisable(AController* OldController);
	UFUNCTION(BlueprintCallable)
	void ControlSystemPossess(AActor* PossessInstigator);
};
