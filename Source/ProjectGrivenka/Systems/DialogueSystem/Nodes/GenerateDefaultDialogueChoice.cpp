// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateDefaultDialogueChoice.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"
#include "ProjectGrivenka/Systems/ControlSystem/Controllable.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/Systems/SmithSystem/SmithSystemAvailable.h"

UGenerateDefaultDialogueChoice::UGenerateDefaultDialogueChoice() {
	this->NodeName = TEXT("Generate Default Dialogue Choice");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UGenerateDefaultDialogueChoice::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return EBTNodeResult::Failed;
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 1, FColor::Cyan, "ffeee");

	TArray<FReplyData> ReplyData;
	FReplyData Talk, Trade, Possess, InviteParty, Smith, End;
	Talk.ReplyId = "TALK";
	Talk.Text = FText::FromString("Got any news?");
	Trade.ReplyId = "TRADE";
	Trade.Text = FText::FromString("Let's trade");
	Possess.ReplyId = "POSSESS";
	Possess.Text = FText::FromString("Take over for a while");
	InviteParty.ReplyId = "INVITE";
	InviteParty.Text = FText::FromString("Wanna come with me?");
	End.ReplyId = "END";
	End.Text = FText::FromString("Nevermind");
	Smith.ReplyId = "SMITH";
	Smith.Text = FText::FromString("Can you upgrade my equipments?");

	APawn* DialogueReceiver = OwnerComp.GetAIOwner()->GetPawn();

	ReplyData.Add(Talk);
	if (DialogueReceiver->Implements<UControllable>()) {
		ReplyData.Add(Possess);
	}
	if (DialogueReceiver->Implements<UAIContextSystemAvailable>()) {
		ReplyData.Add(InviteParty);
	}
	if (DialogueReceiver->Implements<USmithSystemAvailable>()) {
		ReplyData.Add(Smith);
	}
	ReplyData.Add(End);

	GameInstance->UIManager->ShowDialogueReply(ReplyData, this->DialogueQuery);
	GameInstance->UIManager->DialogueReplyDelegate.AddDynamic(this, &UGenerateDefaultDialogueChoice::ReplyChosen);
	return EBTNodeResult::InProgress;
}

void UGenerateDefaultDialogueChoice::ReplyChosen(FName ReplyId)
{
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(this->GetOuter());
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!BTComp || !GameInstance) return;

	BTComp->GetBlackboardComponent()->SetValueAsName(this->GetSelectedBlackboardKey(), ReplyId);
	GameInstance->UIManager->DialogueReplyDelegate.RemoveDynamic(this, &UGenerateDefaultDialogueChoice::ReplyChosen);
	this->FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);

}
