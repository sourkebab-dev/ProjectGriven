// Fill out your copyright notice in the Description page of Project Settings.


#include "InviteToParty.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemAvailable.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystem.h"

UInviteToParty::UInviteToParty() {
	this->NodeName = TEXT("Invite to party");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UInviteToParty::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* DialogueReceiver = OwnerComp.GetAIOwner()->GetPawn();
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return EBTNodeResult::Failed;
	if (!DialogueReceiver->Implements<UDialogueSystemAvailable>()) return EBTNodeResult::Failed;
	if (GameInstance->PartyInstance.Find(DialogueReceiver) != INDEX_NONE) return EBTNodeResult::Failed;
	GameInstance->PartyInstance.Add(DialogueReceiver);

	auto DlgSys = IDialogueSystemAvailable::Execute_GetDialogueSystemComp(DialogueReceiver);
	//note: this needs to be a pending actions because the bt got destroyed on end dialogue
	DlgSys->SetPendingActions(EDialoguePendingActions::INVITE);
	return EBTNodeResult::Succeeded;
}
