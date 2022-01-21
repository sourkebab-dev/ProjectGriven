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
	TEnumAsByte<EAIStateType> DefaultAIState = EAIStateType::IDLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EHostilityType> HostilityType = EHostilityType::NEUTRAL;

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
	static bool CheckHostility(AActor* SourceActor, AActor* HostilityToCheck);

	UFUNCTION(BlueprintCallable)
	void OnContextSetup();
	UFUNCTION()
	void SetRotationRate(EAnimEvt InAnimEvt);
	UFUNCTION()
	virtual void OnHit(AActor* DamageInstigator, FDamageInfo InDamageInfo);
	virtual void SightRefresh();
	virtual void AggroRefresh();
	virtual void OnActorSeen(AActor* SeenActor);
	virtual void ChangeAIState(TEnumAsByte<EAIStateType> NewAIState);
	virtual void AddAggroActor(AActor* AggroInstigator, float AggroPoints);
	virtual void SetAggroTarget(AActor* AggroInstigator);
	virtual void RemoveActorFromAggroList(AActor* AggroInstigator);
	virtual void BTStart();
	virtual void Dodge();
	virtual void Attack();
	virtual void HeavyAttackCharge();
	virtual void HeavyAttackRelease();
	virtual void SetBBAggroTarget(AActor* NewAggroTarget);
	virtual void SendAIEvent(TEnumAsByte<EAIEvent> InAIEvent);


	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
