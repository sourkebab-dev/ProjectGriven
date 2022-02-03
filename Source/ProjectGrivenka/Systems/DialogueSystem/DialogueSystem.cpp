// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/SmithSystem/SmithSystemAvailable.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/Systems/ControlSystem/Controllable.h"
#include "ProjectGrivenka/Systems/ControlSystem/ControlSystem.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/AI/BaseAIController.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

void UDialogueSystem::Init_Implementation()
{
	Super::Init_Implementation();
}

void UDialogueSystem::StartDefaultDialogue()
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance || !this->CompContext->Controller) return;
	
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

	GameInstance->UIManager->OpenDialogueBox();
	auto AIController = Cast<AAIController>(this->CompContext->Controller);
	if (!AIController) return;
	AIController->RunBehaviorTree(this->DefaultDialogueTree);
}

void UDialogueSystem::StopDialogue()
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance || !this->CompContext->Controller) return;
	GameInstance->UIManager->CloseDialogueBox();
	ABaseAIController* AIController = Cast<ABaseAIController>(this->CompContext->Controller);
	if (!AIController) return;
	AIController->BTStart();
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;

	//sponge: shit code tightcoupling
	if (this->PendingAction == EDialoguePendingActions::SWITCH) {
		auto CtrlSys = IControllable::Execute_GetControlSystemComp(this->CompContext->CharacterActor);
		CtrlSys->ControlSystemPossess(this->GetWorld()->GetFirstPlayerController()->GetPawn());
	}
	else if (this->PendingAction == EDialoguePendingActions::INVITE) {
		FCommandInfo CmdInfo;
		CmdInfo.CommandTargetActor = this->GetWorld()->GetFirstPlayerController()->GetPawn();
		CmdInfo.CommandType = EAICommandType::DEFEND;
		IAIContextSystemAvailable::Execute_OnCommanded(this->CompContext->CharacterActor, CmdInfo.CommandTargetActor, CmdInfo);
	}
	else if (this->PendingAction == EDialoguePendingActions::SMITH) {
		ISmithSystemAvailable::Execute_ReceiveSmithRequest(this->CompContext->CharacterActor);
	}
	this->PendingAction = EDialoguePendingActions::NONE;
}

void UDialogueSystem::SetPendingActions(EDialoguePendingActions InActionType)
{
	this->PendingAction = InActionType;
}


