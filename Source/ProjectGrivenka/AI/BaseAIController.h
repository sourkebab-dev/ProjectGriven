// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BTree;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(EditAnywhere)
	FCharacterContext ActorCtx;

	virtual void OnPossess(APawn* PossesedPawn) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void BTStart();
	virtual void Dodge();
	virtual void Attack();
	virtual void HeavyAttackCharge();
	virtual void HeavyAttackRelease();
	virtual void SetBBAggroTarget(AActor* NewAggroTarget);
	virtual void SetBBCharacterState(FGameplayTagContainer NewState);

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
