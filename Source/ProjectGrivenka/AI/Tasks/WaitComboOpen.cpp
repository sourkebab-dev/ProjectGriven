// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitComboOpen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/Character/BaseCharacter.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystem.h"

UWaitComboOpen::UWaitComboOpen() {
	this->NodeName = TEXT("Wait Combo Open");
	this->bCreateNodeInstance = true;
	this->bNotifyTick = 1;
}

EBTNodeResult::Type UWaitComboOpen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	this->CurrentPooledTime = 0.0;
	this->CurrentPaddingTime = 0.0;
	return EBTNodeResult::InProgress;
}
//SPONGE: might be perf issue (should be a latent task)
void UWaitComboOpen::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//this->CurrentPooledTime += DeltaSeconds;
	//ABaseCharacter* CharacterInstance = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(this->GetSelectedBlackboardKey()));
	//IContextAvailable* CtxAvail = Cast<IContextAvailable>(CharacterInstance);
	//FCharacterContext CharCtx;
	//CtxAvail->Execute_GetContext(Cast<UObject>(CtxAvail), CharCtx);
	/*
	if (CharacterInstance && CharCtx.CharacterSystemComp->CombatFlags.IsComboActive) {
		this->CurrentPaddingTime += DeltaSeconds;
		if (this->CurrentPaddingTime >= this->TargetPaddingTime) {
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}else if (this->CurrentPooledTime > this->DefaultExitTime) {
		UE_LOG(LogTemp, Error, TEXT("Failed to wait for combo open"), *GetNameSafe(this));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}	*/
}