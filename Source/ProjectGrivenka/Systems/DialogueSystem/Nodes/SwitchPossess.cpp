// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchPossess.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemAvailable.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystem.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

USwitchPossess::USwitchPossess() {
	this->NodeName = TEXT("Switch to this character (Contains End Dialogue)");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type USwitchPossess::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* DialogueReceiver = OwnerComp.GetAIOwner()->GetPawn();
		
	if (!DialogueReceiver->Implements<UDialogueSystemAvailable>()) return EBTNodeResult::Failed;
	auto DlgSys = IDialogueSystemAvailable::Execute_GetDialogueSystemComp(DialogueReceiver);
	//note: this needs to be a pending actions because switching from dialogue controller to another controller is fucking up the input component for some reason
	DlgSys->SetPendingActions(EDialoguePendingActions::SWITCH);
	return EBTNodeResult::Succeeded;
}
