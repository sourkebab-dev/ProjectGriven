// Fill out your copyright notice in the Description page of Project Settings.


#include "Smithing.h"
#include "AIController.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemAvailable.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystem.h"

EBTNodeResult::Type USmithing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* DialogueReceiver = OwnerComp.GetAIOwner()->GetPawn();
	if (!DialogueReceiver->Implements<UDialogueSystemAvailable>()) return EBTNodeResult::Failed;
	auto DlgSys = IDialogueSystemAvailable::Execute_GetDialogueSystemComp(DialogueReceiver);
	//SPONGE: THIS PENDING SHIT IS HORRIBELE
	DlgSys->SetPendingActions(EDialoguePendingActions::SMITH);
	return EBTNodeResult::Succeeded;
}
