// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AIController.h"
#include "Controllable.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

UControlSystem::UControlSystem() : UBaseContextableComponent() {
	this->PrimaryComponentTick.bCanEverTick = true;
}

void UControlSystem::Init_Implementation()
{
	Super::Init_Implementation();
	if (!this->CompContext->EventBus) {
		UE_LOG(LogTemp, Error, TEXT("Eventbus Initiation Failure"), *GetNameSafe(this)); return;
	}
	this->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UControlSystem::AnimHandler);
	APawn* ActorPawn = Cast<APawn>(this->CompContext->CharacterActor);
	if (!ActorPawn || !this->CompContext->CharacterActor->Implements<UControllable>()) {
		UE_LOG(LogTemp, Error, TEXT("ControlSystem Initiation Failure (Owner must be a pawn) & Implements IControllable"), *GetNameSafe(this)); return;
	}

	this->GI = Cast<UBaseGameInstance>(this->GetWorld()->GetGameInstance());
	this->SpringArm = IControllable::Execute_GetCameraSpringArm(this->CompContext->CharacterActor);
	this->CameraMain = IControllable::Execute_GetMainCamera(this->CompContext->CharacterActor);
}

//Note: Called on Pawn Possessed
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
	InputComp->BindAction("Inventory", IE_Pressed, this, &UControlSystem::ControlInventory);
	InputComp->BindAction("Command1", IE_Pressed, this, &UControlSystem::ControlCommand1);
	InputComp->BindAction("Command2", IE_Pressed, this, &UControlSystem::ControlCommand2);
	InputComp->BindAction("Command3", IE_Pressed, this, &UControlSystem::ControlCommand3);
	InputComp->BindAction("CommandCancel", IE_Pressed, this, &UControlSystem::ControlCommandCancel);
	InputComp->BindAction("LockOn", IE_Pressed, this, &UControlSystem::ControlLockOn);
	InputComp->BindAction("Crouch", IE_Pressed, this, &UControlSystem::ControlCrouch);
	InputComp->BindAction("Jump", IE_Pressed, this, &UControlSystem::ControlJump);
	InputComp->BindAxis("MoveForward", this, &UControlSystem::ControlMoveForward);
	InputComp->BindAxis("MoveRight", this, &UControlSystem::ControlMoveRight);
	InputComp->BindAxis("CycleItem", this, &UControlSystem::ControlCycleItem);
	InputComp->BindAxis("MouseX", this, &UControlSystem::ControlMouseX);
	InputComp->BindAxis("MouseY", this, &UControlSystem::ControlMouseY);
	this->SetComponentTickEnabled(true);
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 1, FColor::Cyan, "Enable");

}

//Note: Called on Pawn UnPossessed
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
	InputComp->RemoveActionBinding("LockOn", IE_Pressed);
	InputComp->RemoveActionBinding("Inventory", IE_Pressed);
	InputComp->RemoveActionBinding("Crouch", IE_Pressed);
	InputComp->RemoveActionBinding("Jump", IE_Pressed);
	InputComp->AxisBindings.Empty();
	this->SetComponentTickEnabled(false);

}

void UControlSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//sponge: change pers deprecated code
	AController* PlayerController = this->GetWorld()->GetFirstPlayerController();
	if (!PlayerController) {
		GLog->Log("ControlSystem RotateTick PlayerController Failure");
		return;
	}

	FVector TargetLocRot = FVector::ZeroVector;
	if (this->LockedActor) {
		TargetLocRot = this->LockedActor->GetActorLocation();
	}
	else {
		TargetLocRot = this->CompContext->MovementModule.WorldSpaceTargetDir;
		if (TargetLocRot.IsZero()) {
			TargetLocRot = this->CompContext->CharacterActor->GetActorForwardVector();
		}
		TargetLocRot = this->CompContext->CharacterActor->GetActorLocation() + TargetLocRot  * 10;
	}

	UVectorMathLib::RotateActorToTargetVector(this->GetOwner(), TargetLocRot, this->RotationRate, DeltaTime);
}

void UControlSystem::AnimHandler(EAnimEvt InAnimEvt)
{
	switch (InAnimEvt)
	{
	case ACTIVATE_INTERRUPTION:
		this->ProcessBufferedActions();
		break;
	case ENABLE_INPUT_BUFFER:
		this->IsBuffering = true;
		break;
	case DISABLE_INPUT_BUFFER:
		this->IsBuffering = false;
		break;
	case FULL_ROTATION:
		this->RotationRate = 8;
		break;
	case SLOW_ROTATION:
		this->RotationRate = 5;
		break;
	case OFF_ROTATION:
		this->RotationRate = 0.01;
		break;
	default:
		break;
	}
}

void UControlSystem::UpdateWorldSpaceVectors() {
	float YawRotation = this->SpringArm->GetComponentRotation().Yaw;
	FRotator TempRotator = FRotator(0, YawRotation, 0);
	this->CompContext->MovementModule.WorldSpaceTargetDir = TempRotator.RotateVector(this->RawInput);
}

void UControlSystem::ControlSystemSwitchPossess(AActor* PossessInstigator)
{
	APawn* OwnerPawn = Cast<APawn>(this->CompContext->CharacterActor);
	if (!OwnerPawn) return;

	this->GetWorld()->GetFirstPlayerController()->UnPossess();
	auto OwnerController = OwnerPawn->GetController();
	OwnerController->UnPossess();
	if(OwnerController) OwnerController->Destroy();

	auto InstigatorPawn = Cast<APawn>(PossessInstigator);
	InstigatorPawn->SpawnDefaultController();

	this->GetWorld()->GetFirstPlayerController()->Possess(OwnerPawn);
	this->GI->PartyInstance.Empty();
	this->GI->UIManager->StatsSetup();

	//sponge: need to set character id somewhere
	//GameInstance->SetControlledCrewId(this->CharacterId);
}

void UControlSystem::BufferAction(FBufferedAction InBufferedAction)
{
	bool IsClearBuffer = false;
	bool IsSaveAction = true;
	for (int i = 0; i < this->BufferedActions.Num(); i++) {
		// Check for different action, if so, replace old inputs
		if (this->BufferedActions[i].Action != InBufferedAction.Action) {
			IsClearBuffer = true;
			break;
		}

		// Check for duplicate input
		if (this->BufferedActions[i].Action == InBufferedAction.Action && this->BufferedActions[i].EventType == InBufferedAction.EventType) {
			IsSaveAction = false;
			break;
		}
	}

	if (IsClearBuffer) this->BufferedActions.Empty();
	if (IsSaveAction) this->BufferedActions.Add(InBufferedAction);
}

void UControlSystem::ProcessBufferedActions()
{
	this->IsBuffering = false;
	for (int i = 0; i < this->BufferedActions.Num(); i++) {
		this->CompContext->EventBus->StateActionDelegate.Broadcast(this->BufferedActions[i].Action, this->BufferedActions[i].EventType);
	}
	this->BufferedActions.Empty();
}

void UControlSystem::ControlCycleItem(float Value)
{
}

void UControlSystem::ControlMouseX(float Value)
{
	FRotator TempRotator = FRotator(0, Value, 0);
	this->SpringArm->AddRelativeRotation(TempRotator);
}

void UControlSystem::ControlMouseY(float Value)
{
	FRotator TempRotator = FRotator(Value, 0, 0);
	this->SpringArm->AddRelativeRotation(TempRotator);
	float ClampedPitch = FMath::Clamp(this->SpringArm->GetRelativeRotation().Pitch, -60.0f, 60.0f);
	this->SpringArm->SetRelativeRotation(FRotator(ClampedPitch, this->SpringArm->GetRelativeRotation().Yaw, 0.0));
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
	if (this->IsBuffering) {
		FBufferedAction BA;
		BA.Action = EActionList::ActionAttack;
		BA.EventType = IE_Pressed;
		this->BufferAction(BA);
	}
	else {
		this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
	}
}

void UControlSystem::ControlAttackRelease()
{
	if (this->IsBuffering) {
		FBufferedAction BA;
		BA.Action = EActionList::ActionAttack;
		BA.EventType = IE_Released;
		this->BufferAction(BA);
	}
	else {
		this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
	}
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
	if (this->IsBuffering) {
		FBufferedAction BA;
		BA.Action = EActionList::ActionDodge;
		BA.EventType = IE_Pressed;
		this->BufferAction(BA);
	}
	else {
		this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionDodge, IE_Pressed);
	}
}

void UControlSystem::ControlInteract()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionInteract, IE_Pressed);
}

void UControlSystem::ControlInventory()
{
	if (this->CompContext->CharacterActor->Implements<UCharacterInventoryAvailable>()) {
		ICharacterInventoryAvailable::Execute_ToggleShowInventory(this->CompContext->CharacterActor);
	}
}

void UControlSystem::ControlCrouch()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionCrouch, IE_Pressed);
}

void UControlSystem::ControlJump()
{
	this->CompContext->EventBus->StateActionDelegate.Broadcast(EActionList::ActionJump, IE_Pressed);
}


void UControlSystem::ControlLockOn()
{

	if (this->LockedActor) {
		this->LockedActor = nullptr;
		return;
	}

	FVector CameraLoc = this->CameraMain->GetComponentLocation();
	FVector CameraForwardVec = this->CameraMain->GetForwardVector();
	FVector RotateAxis = CameraForwardVec.RotateAngleAxis(-90, FVector::UpVector);
	FVector TargetHit = CameraLoc +  CameraForwardVec.RotateAngleAxis(7.0f, RotateAxis) * 10000;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this->CompContext->CharacterActor);
	FHitResult HitResult;

	if (UKismetSystemLibrary::SphereTraceSingle(this->GetWorld(), CameraLoc, TargetHit, 30.0f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true)) {
		this->LockedActor = HitResult.Actor.Get();
	}
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

