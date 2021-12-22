// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowReply.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

UShowReply::UShowReply() {
	this->NodeName = TEXT("Reply Dialogue");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UShowReply::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return EBTNodeResult::Failed;

	GameInstance->UIManager->ShowDialogueReply(this->ReplyData, this->DialogueQuery);
	GameInstance->UIManager->DialogueReplyDelegate.AddDynamic(this, &UShowReply::ReplyChosen);
	return EBTNodeResult::InProgress;
}


void UShowReply::ReplyChosen(FName ReplyId)
{
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(this->GetOuter());
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!BTComp || !GameInstance) return;

	BTComp->GetBlackboardComponent()->SetValueAsName(this->GetSelectedBlackboardKey(), ReplyId);
	GameInstance->UIManager->DialogueReplyDelegate.RemoveDynamic(this, &UShowReply::ReplyChosen);
	this->FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
}