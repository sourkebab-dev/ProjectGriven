// Fill out your copyright notice in the Description page of Project Settings.


#include "DoSpecialAttacks.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/AI/BaseAIController.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"

UDoSpecialAttacks::UDoSpecialAttacks() {
	this->NodeName = TEXT("Do Special Attacks(Temp)");
	this->bCreateNodeInstance = true;
	this->bNotifyTick = 1;
}

EBTNodeResult::Type UDoSpecialAttacks::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	this->CurrentPooledTime = 0.0;

	if (!OwnerComp.GetAIOwner()) return EBTNodeResult::Failed;
	APawn* ThisPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ThisPawn) {

		if (!ThisPawn->Implements<UContextAvailable>()) return EBTNodeResult::Failed;
		FCharacterContext CharCtx;
		IContextAvailable::Execute_GetContext(ThisPawn, CharCtx);
		if (CharCtx.CharacterAnim) {
			CharCtx.CharacterAnim->Montage_Play(this->AttackMontage);
			this->MontageLength = this->AttackMontage->SequenceLength;
			return EBTNodeResult::InProgress;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Invalid Character Instance"), *GetNameSafe(this));
		}
	}
	return EBTNodeResult::Failed;
}

void UDoSpecialAttacks::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	this->CurrentPooledTime += DeltaSeconds;
	if (this->CurrentPooledTime >= this->MontageLength) {
		//SPONGE: TEMPORARY BAD CODE
		if (!OwnerComp.GetAIOwner()) this->FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		APawn* ThisPawn = OwnerComp.GetAIOwner()->GetPawn();
		if (ThisPawn) {
			//ABaseCharacter* CharacterIns = Cast<ABaseCharacter>(ThisPawn);
			//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
			//if (CharacterIns && CommonData) {
				//CharacterIns->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
				//this->FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			//}
			//else {
			//	UE_LOG(LogTemp, Error, TEXT("Invalid Character Instance"), *GetNameSafe(this));
			//}
		}

		this->FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}