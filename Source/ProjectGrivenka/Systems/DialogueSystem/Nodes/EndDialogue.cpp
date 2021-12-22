// Fill out your copyright notice in the Description page of Project Settings.


#include "EndDialogue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectGrivenka/Systems/DialogueSystem/DialogueSystemAvailable.h"

UEndDialogue::UEndDialogue() {
	this->NodeName = TEXT("End Dialogue");
	this->bCreateNodeInstance = true;
}

EBTNodeResult::Type UEndDialogue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetBlackboardComponent()->GetValueAsObject(this->GetSelectedBlackboardKey())->Implements<UDialogueSystemAvailable>()) return EBTNodeResult::Failed;
	IDialogueSystemAvailable::Execute_GetDialogueSystemComp(OwnerComp.GetBlackboardComponent()->GetValueAsObject(this->GetSelectedBlackboardKey()))->StopDialogue();
	return EBTNodeResult::Succeeded;
}
