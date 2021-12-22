// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowDialogue.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

UShowDialogue::UShowDialogue() {
	this->NodeName = TEXT("Show Dialogue");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UShowDialogue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return EBTNodeResult::Failed;

	this->CurrDialogueIndex = 0;
	this->DisplayLine();
	GameInstance->UIManager->DialogueProceedDelegate.AddDynamic(this, &UShowDialogue::ProceedDialogue);
	return EBTNodeResult::InProgress;
}

void UShowDialogue::DisplayLine()
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance || !GameInstance->UIManager) return;
	GameInstance->UIManager->ShowDialogue(this->DialogueData[this->CurrDialogueIndex]);
}

void UShowDialogue::ProceedDialogue()
{
	if (this->CurrDialogueIndex < this->DialogueData.Num() - 1) {
		this->CurrDialogueIndex += 1;
		this->DisplayLine();
	}
	else {
		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(this->GetOuter());
		if (!BTComp) return;

		UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
		if (GameInstance) {
			GameInstance->UIManager->DialogueProceedDelegate.RemoveDynamic(this, &UShowDialogue::ProceedDialogue);
		}
		this->FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
		
	}
}
