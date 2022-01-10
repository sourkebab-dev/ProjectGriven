// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectGrivenka/Interfaces/Interactable.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemAvailable.h"
#include "ProjectGrivenka/PersistedData/Persistable.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLatentActionDelegate);

UCLASS()
class PROJECTGRIVENKA_API ABaseCharacter : public ACharacter, public IInteractable, public IDialogueSystemAvailable, public IContextAvailable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterContext CharacterContext;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Events")
	FLatentActionDelegate LatentActionDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector TargetMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* RotateTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class ABaseAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TArray<ABaseCharacter*> AggroList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ABaseCharacter* AggroTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	class UBoxComponent* InteractionCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	AActor* InteractableItem;


protected:
#pragma region Lifecycles
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
#pragma endregion

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SaveData();
	virtual void SaveData_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadData();
	virtual void LoadData_Implementation();

	UFUNCTION()
	virtual void SaveInteractable(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void RemoveInteractable(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void Interact_Implementation(AActor* InteractInstigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CheckIsInteractable(AActor* InteractInstigator);
	virtual bool CheckIsInteractable_Implementation(AActor* InteractInstigator) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RequestInteractionInfo(AActor* InteractionInstigator, FInteractionInfo& OutInteractionInfo);
	virtual void RequestInteractionInfo_Implementation(AActor* InteractInstigator, FInteractionInfo& OutInteractionInfo) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetContext(FCharacterContext& OutContext);
	virtual void GetContext_Implementation(FCharacterContext& OutContext) override;

	UFUNCTION(BlueprintCallable)
	void RotateToTarget();
	
	void AISetAggroTarget(ABaseCharacter* NewAggroTarget);

	void AISetAggroTargetFromList();

	void AISet(class ABaseAIController* NewController);

	void AIRemove();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABaseCharacter* GetAggroTarget() { return AggroTarget; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector GetTargetMovementDirection() { return TargetMovementDirection; }

	UFUNCTION(BlueprintCallable)
	void SetTargetMovementDirection(FVector NewTargetDir) { TargetMovementDirection = NewTargetDir; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetRotationRate() { return RotationRate; }

	UFUNCTION(BlueprintCallable)
	void SetRotationRate(float NewRotationRate) { RotationRate = NewRotationRate; }

#pragma region Anim BP Integration
	UFUNCTION(BlueprintImplementableEvent)
	void AnimStartDodge();

	UFUNCTION(BlueprintImplementableEvent)
	void AnimFinishDodge();
#pragma endregion


#pragma region State Functional Things
	//SPONGE: maybe add struct for state info shit

#pragma endregion

};
