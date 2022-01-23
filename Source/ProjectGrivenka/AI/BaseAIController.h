// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EAIEvent
{
	AI_NONE,
	SIGNALLED_ATTACK,
};

UCLASS()
class PROJECTGRIVENKA_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

	ABaseAIController();

public:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BTree;

	UPROPERTY(BlueprintReadWrite)
	class UContextSystem* ActorCtx;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<AActor*, float> AggroMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* AggroTarget;

	float RotationRate = 20;
	FTimerHandle AggroRefreshTimer;
	FTimerHandle SightRefreshTimer;

	virtual void OnPossess(APawn* PossesedPawn) override;
	virtual void OnUnPossess() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(BlueprintCallable)
	void OnContextSetup();
	UFUNCTION()
	void SetRotationRate(EAnimEvt InAnimEvt);
	void ChangeAIState(TEnumAsByte<EAIStateType> NewAIState);
	void SendAIEvent(TEnumAsByte<EAIEvent> InAIEvent);
	void SightRefresh();
	void AggroRefresh();
	void AddAggroActor(AActor* AggroInstigator, float AggroPoints);
	void SetAggroTarget(AActor* AggroInstigator);
	void RemoveActorFromAggroList(AActor* AggroInstigator);
	void BTStart();
	void Dodge();
	void Attack();
	void HeavyAttackCharge();
	void HeavyAttackRelease();
	void SetBBDefendActor(AActor* DefendActor);
	void SetBBMovementLocation(FVector TargetLocation);
	virtual void OnActorSeen(AActor* SeenActor);
	UFUNCTION()
	virtual void OnHit(AActor* DamageInstigator, FDamageInfo InDamageInfo);


	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
