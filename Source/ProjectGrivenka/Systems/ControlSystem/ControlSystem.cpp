// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "Controllable.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"

UControlSystem::UControlSystem() : UBaseContextableComponent() {
	this->PrimaryComponentTick.bCanEverTick = true;
}

void UControlSystem::Init()
{
	Super::Init();
	if (!this->CompContext->EventBus) {
		UE_LOG(LogTemp, Error, TEXT("Eventbus Initiation Failure"), *GetNameSafe(this)); return;
	}
	this->CompContext->EventBus->PossessionDelegate.AddDynamic(this, &UControlSystem::ControlSystemSetup);
	this->CompContext->EventBus->UnpossesionDelegate.AddDynamic(this, &UControlSystem::ControlSystemDisable);
	this->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UControlSystem::AnimHandler);
	APawn* ActorPawn = Cast<APawn>(this->CompContext->CharacterActor);
	if (!ActorPawn || !this->CompContext->CharacterActor->Implements<UControllable>()) {
		UE_LOG(LogTemp, Error, TEXT("ControlSystem Initiation Failure (Owner must be a pawn) & Implements IControllable"), *GetNameSafe(this)); return;
	}
	this->ControlSystemSetup(ActorPawn->GetController());
	this->GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
}

void UControlSystem::ControlSystemSetup(AController* NewController)
{	
	if (!NewController || !NewController->IsPlayerController() || (!this->GetOwner()->InputComponent && !NewController->InputComponent)) {
		this->SetComponentTickEnabled(false);
		return;
	}
	this->CompContext->Controller = NewController;
	UInputComponent* InputComp = this->GetOwner()->InputComponent ? this->GetOwner()->InputComponent : NewController->InputComponent;
	InputComp->BindAction("Interact", IE_Pressed, this, &UControlSystem::ControlInteract);
	InputComp->BindAction("Attack", IE_Pressed, this, &UControlSystem::ControlAttack);
	InputComp->BindAction("Attack", IE_Released, this, &UControlSystem::ControlAttackRelease);
	InputComp->BindAction("Block", IE_Pressed, this, &UControlSystem::ControlBlock);
	InputComp->BindAction("Block", IE_Released, this, &UControlSystem::ControlBlockRelease);
	InputComp->BindAction("Dodge", IE_Pressed, this, &UControlSystem::ControlDodge);
	InputComp->BindAction("ToggleAmpField", IE_Pressed, this, &UControlSystem::ControlToggleAmpField);
	InputComp->BindAction("VentAmp", IE_Pressed, this, &UControlSystem::ControlVentAmp);
	InputComp->BindAction("UseItem", IE_Pressed, this, &UControlSystem::ControlUseItem);
	InputComp->BindAction("Command1", IE_Pressed, this, &UControlSystem::ControlCommand1);
	InputComp->BindAction("Command2", IE_Pressed, this, &UControlSystem::ControlCommand2);
	InputComp->BindAction("Command3", IE_Pressed, this, &UControlSystem::ControlCommand3);
	InputComp->BindAction("CommandCancel", IE_Pressed, this, &UControlSystem::ControlCommandCancel);
	InputComp->BindAxis("MoveForward", this, &UControlSystem::ControlMoveForward);
	InputComp->BindAxis("MoveRight", this, &UControlSystem::ControlMoveRight);
	InputComp->BindAxis("CycleItem", this, &UControlSystem::ControlCycleItem);
	this->SetComponentTickEnabled(true);
	Cast<APlayerController>(NewController)->bShowMouseCursor = true;

}

void UControlSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AController* PlayerController = this->GetWorld()->GetFirstPlayerController();
	if (!PlayerController) {
		GLog->Log("ControlSystem RotateTick PlayerController Failure");
		return;
	}
	FHitResult MouseHit;
	this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);
	UVectorMathLib::RotateActorToTargetVector(this->GetOwner(), MouseHit.Location, this->RotationRate, DeltaTime);
}

void UControlSystem::AnimHandler(EAnimEvt InAnimEvt)
{
	switch (InAnimEvt)
	{
	case FULL_ROTATION:
		this->RotationRate = 20;
		break;
	case SLOW_ROTATION:
		this->RotationRate = 8;
		break;
	case OFF_ROTATION:
		this->RotationRate = 0.01;
		break;
	default:
		break;
	}
}

void UControlSystem::UpdateWorldSpaceVectors() {
	float YawRotation = IControllable::Execute_GetControlledCameraYawRotation(this->CompContext->CharacterActor);
	FRotator TempRotator = FRotator(0, YawRotation, 0);
	this->CompContext->MovementModule.WorldSpaceTargetDir = TempRotator.RotateVector(this->RawInput);
}

void UControlSystem::ControlSystemDisable(AController* OldController)
{
	if (!OldController || !OldController->IsPlayerController() || (!this->GetOwner()->InputComponent && !OldController->InputComponent)) return;
	this->CompContext->Controller = nullptr;
	UInputComponent* InputComp = this->GetOwner()->InputComponent ? this->GetOwner()->InputComponent : OldController->InputComponent;
	InputComp->RemoveActionBinding("Interact", IE_Pressed);
	InputComp->RemoveActionBinding("Attack", IE_Pressed);
	InputComp->RemoveActionBinding("Attack", IE_Released);
	InputComp->RemoveActionBinding("Block", IE_Pressed);
	InputComp->RemoveActionBinding("Block", IE_Released);
	InputComp->RemoveActionBinding("Dodge", IE_Pressed);
	InputComp->RemoveActionBinding("ToggleAmpField", IE_Pressed);
	InputComp->RemoveActionBinding("VentAmp", IE_Pressed);
	InputComp->RemoveActionBinding("UseItem", IE_Pressed);
	InputComp->RemoveActionBinding("Command1", IE_Pressed);
	InputComp->RemoveActionBinding("Command2", IE_Pressed);
	InputComp->RemoveActionBinding("Command3", IE_Pressed);
	InputComp->RemoveActionBinding("CommandCancel", IE_Pressed);
	InputComp->AxisBindings.Empty();
	this->SetComponentTickEnabled(false);
}

void UControlSystem::ControlSystemPossess(AActor* PossessInstigator)
{
	APawn* OwnerPawn = Cast<APawn>(this->CompContext->CharacterActor);
	if (!OwnerPawn) return;


	this->GetWorld()->GetFirstPlayerController()->UnPossess();
	OwnerPawn->GetController()->UnPossess();
	if(OwnerPawn->GetController()) OwnerPawn->GetController()->Destroy();

	if (PossessInstigator) {
		auto InstigatorCtx = IContextAvailable::Execute_GetContext(PossessInstigator);
		AController* PlayerController = InstigatorCtx->Controller;
		Cast<APawn>(PossessInstigator)->SpawnDefaultController();
	}

	this->GetWorld()->GetFirstPlayerController()->Possess(OwnerPawn);

	//sponge: need to set character id somewhere
	//GameInstance->SetControlledCrewId(this->CharacterId);
}

void UControlSystem::ControlCycleItem(float Value)
{
}

void UControlSystem::ControlMoveForward(float Value)
{
	this->RawInput.X = Value;
	this->RawInput.Normalize();
	this->UpdateWorldSpaceVectors();
	this->CompContext->EventBus->StateAxisDelegate.Broadcast(EActionList::ActionMoveForward, Value);
}

void UControlSystem::ControlMoveRight(float Value)
{
	this->RawInput.Y = Value;
	this->RawInput.Normalize();
	this->UpdateWorldSpaceVectors();
	this->CompContext->EventBus->StateAxisDelegate.Broadcast(EActionList::ActionMoveRight, Value);
}

void UControlSystem::ControlAttack()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
}

void UControlSystem::ControlAttackRelease()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
}

void UControlSystem::ControlBlock()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionBlock, IE_Pressed);
}

void UControlSystem::ControlBlockRelease()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionBlock, IE_Released);
}

void UControlSystem::ControlToggleAmpField()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionToggleAmpField, IE_Pressed);
}

void UControlSystem::ControlUseItem()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionUseItem, IE_Pressed);
}

void UControlSystem::ControlVentAmp()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionVentAmp, IE_Pressed);
}

void UControlSystem::ControlDodge()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionDodge, IE_Pressed);
}

void UControlSystem::ControlInteract()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionInteract, IE_Pressed);
}

void UControlSystem::ControlCommand1()
{

	if (this->CommandedActor) {
		auto CmdCtx = IContextAvailable::Execute_GetContext(this->CommandedActor);
		FHitResult CommandHit, MouseHit;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this->CommandedActor);
		this->GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);

		FCommandInfo CmdInfo;
		if (UKismetSystemLibrary::SphereTraceSingle(this->GetWorld(), MouseHit.Location, MouseHit.Location, 50.0f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, CommandHit, true )) {
			AActor* HitActor = CommandHit.Actor.Get();
			if (HitActor->Implements<UAIContextSystemAvailable>()) {
				CmdInfo.CommandTargetActor = HitActor;
				if (IAIContextSystemAvailable::Execute_CheckHostility(this->CommandedActor, HitActor)) {
					CmdInfo.CommandType = EAICommandType::ATTACK;
					GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Attack this target!");

				}
				else {
					CmdInfo.CommandType = EAICommandType::DEFEND;
					GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Defend this target!");
				}
			}
			else {
				CmdInfo.CommandType = EAICommandType::MOVETO;
				CmdInfo.CommandTargetLocation = HitActor->GetActorLocation();
				GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Move to this position!");
			}
		}
		else {
			CmdInfo.CommandType = EAICommandType::MOVETO;
			CmdInfo.CommandTargetLocation = MouseHit.Location;
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Move to this position!");
		}

		CmdCtx->EventBus->AICommandDelegate.Broadcast(this->CompContext->CharacterActor, CmdInfo);
		this->CommandedActor = nullptr;
	}
	else {
		if (!this->GI->PartyInstance.IsValidIndex(0)) return;
		this->CommandedActor = this->GI->PartyInstance[0];
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Companion 1, Hear me!");
	}
}

void UControlSystem::ControlCommand2()
{
	if (this->CommandedActor) {
		auto CmdCtx = IContextAvailable::Execute_GetContext(this->CommandedActor);
		FCommandInfo CmdInfo;
		CmdInfo.CommandTargetActor = this->CompContext->CharacterActor;
		CmdInfo.CommandType = EAICommandType::DEFEND;
		CmdCtx->EventBus->AICommandDelegate.Broadcast(this->CompContext->CharacterActor, CmdInfo);
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Follow me!");
		this->CommandedActor = nullptr;
	}
	else {
		if (!this->GI->PartyInstance.IsValidIndex(1)) return;
		this->CommandedActor = this->GI->PartyInstance[1];
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Companion 1, Hear me!");
	}
}

void UControlSystem::ControlCommand3()
{
	if (this->CommandedActor) {
		auto CmdCtx = IContextAvailable::Execute_GetContext(this->CommandedActor);
		FHitResult CommandHit;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this->CommandedActor);
		ActorsToIgnore.Add(this->CompContext->CharacterActor);

		if (UKismetSystemLibrary::SphereTraceSingle(this->GetWorld(), this->CommandedActor->GetActorLocation(), this->CommandedActor->GetActorLocation(), 1000.0f, 
			UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, CommandHit, true)) {
			FCommandInfo CmdInfo;
			AActor* HitActor = CommandHit.Actor.Get();
			if (!HitActor->Implements<UAIContextSystemAvailable>() || !IAIContextSystemAvailable::Execute_CheckHostility(this->CommandedActor, HitActor)) return;
			CmdInfo.CommandTargetActor = HitActor;
			CmdInfo.CommandType = EAICommandType::ATTACK;
			CmdCtx->EventBus->AICommandDelegate.Broadcast(this->CompContext->CharacterActor, CmdInfo);
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Charge!");
		}
		this->CommandedActor = nullptr;
	}
	else {
		if (!this->GI->PartyInstance.IsValidIndex(2)) return;
		this->CommandedActor = this->GI->PartyInstance[2];
	}
}

void UControlSystem::ControlCommandCancel()
{
	this->CommandedActor = nullptr;
}
