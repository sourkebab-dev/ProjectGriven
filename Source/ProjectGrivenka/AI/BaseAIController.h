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

public:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BTree;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(EditAnywhere)
	FCharacterContext ActorCtx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAIState> CurrentAIState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EHostilityType> HostilityType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<AActor*, float> AggroMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* AggroTarget;

	FTimerHandle AggroRefreshTimer;
	FTimerHandle SightRefreshTimer;

	virtual void OnPossess(APawn* PossesedPawn) override;
	virtual void OnUnPossess() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void SightRefresh();
	virtual void AggroRefresh();
	virtual void OnActorSeen(AActor* SeenActor);
	virtual bool CheckHostility(AActor* HostilityToCheck);
	virtual void ChangeAIState(TEnumAsByte<EAIState> NewAIState);
	virtual void OnHit(AActor* DamageInstigator, FDamageInfo InDamageInfo);
	virtual void AddAggroActor(AActor* AggroInstigator, float AggroPoints);
	virtual void SetAggroTarget(AActor* AggroInstigator);
	virtual void RemoveActorFromAggroList(AActor* AggroInstigator);

	virtual void BTStart();
	virtual void Dodge();
	virtual void Attack();
	virtual void HeavyAttackCharge();
	virtual void HeavyAttackRelease();
	virtual void SetBBAggroTarget(AActor* NewAggroTarget);
	virtual void SetBBCharacterState(FGameplayTagContainer NewState);

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
