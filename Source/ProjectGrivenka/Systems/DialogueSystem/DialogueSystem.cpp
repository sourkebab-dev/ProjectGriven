// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/AI/BaseAIController.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

void UDialogueSystem::Init()
{
	Super::Init();
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
	this->GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
}


