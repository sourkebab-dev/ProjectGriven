// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/ContextUtilities/BaseContextableComponent.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "CharacterStatesSystem.generated.h"


USTRUCT(BlueprintType)
struct FCrossStateData
{
	GENERATED_BODY()

		UPROPERTY(BlueprintReadWrite)
		bool IsComboActive = true;

	UPROPERTY(BlueprintReadWrite)
		bool IsInterruptable = true;

	UPROPERTY(BlueprintReadWrite)
		bool IsAmpActivated = false;

	UPROPERTY(BlueprintReadWrite)
		bool IsLungeAvailable = false;

	UPROPERTY(BlueprintReadWrite)
		bool IsLungePooling = false;

	UPROPERTY(BlueprintReadWrite)
		bool IsHitStopped = false;

	UPROPERTY(BlueprintReadWrite)
		bool IsParry = false;

	UPROPERTY(BlueprintReadWrite)
		FDamageInfo DamageInfo;

	UPROPERTY(BlueprintReadWrite)
		AActor* DamageInstigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
		UAnimMontage* KnockDownMontage = nullptr;
};


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTGRIVENKA_API UCharacterStatesSystem : public UBaseContextableComponent
{
	GENERATED_BODY()

	UCharacterStatesSystem();

public:
	UPROPERTY(BlueprintReadWrite)
	FCrossStateData CrossStateData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer BlockedTags;

	UPROPERTY(BlueprintReadWrite)
	UBaseState* CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UBaseState>> GrantedActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Knock  & Stagger")
	bool IsStaggeredOnEmptyFortitude = true; // Note: only affects when hit by an attack
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knock  & Stagger")
	TEnumAsByte<EDamageImpactType> HeavyKnockedByWhichImpact = DI_HIGH;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Knock  & Stagger")
	TEnumAsByte<EDamageImpactType> LaunchByWhichImpact = DI_EXPLOSIVE;

	FDamagedDelegate BlockHitDelegate;

protected:
	FTimerHandle AnimLockHandle;

public:	
	void Init_Implementation() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	virtual void GrantAction(TSubclassOf<class UBaseState> InGrantedAction);
	UFUNCTION(BlueprintCallable)
	void RemoveAction(TSubclassOf<class UBaseState> InGrantedAction);
	UFUNCTION(BlueprintCallable)
	virtual void AnimEventsHandler(enum EAnimEvt InAnimEvent);
	UFUNCTION(BlueprintCallable)
	virtual void CurrentActionHandler(EActionList Action, EInputEvent InputEvent);
	UFUNCTION(BlueprintCallable)
	virtual void CurrentAxisHandler(EActionList Action, float AxisValue);
	UFUNCTION(BlueprintCallable)
	virtual bool ChangeState(FGameplayTag ChangeTo, EActionList NewEnterAction, EInputEvent NewEnterEvent);
	UFUNCTION(BlueprintCallable)
	virtual class UBaseState* GetCurrentState() { return CurrentState;  };
	UFUNCTION()
	void OnHit(AActor* HitInstigator, FDamageInfo InDamageInfo);
	UFUNCTION()
	void OnMovementModeChanged(class ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PrevCustomMode);

	void LockAnimation(EDamageImpactType InDamageImpactTime, FHitStopFinishDelegate OnHitFinish);
};
