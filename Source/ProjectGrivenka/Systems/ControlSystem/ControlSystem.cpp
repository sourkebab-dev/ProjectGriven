// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlSystem.h"
#include "AIController.h"
#include "Controllable.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void UControlSystem::Init()
{
	Super::Init();
	if (!this->CompContext.EventBus) {
		UE_LOG(LogTemp, Error, TEXT("Eventbus Initiation Failure"), *GetNameSafe(this)); return;
	}
	this->CompContext.EventBus->PossessionDelegate.AddDynamic(this, &UControlSystem::ControlSystemSetup);
	this->CompContext.EventBus->UnpossesionDelegate.AddDynamic(this, &UControlSystem::ControlSystemDisable);
	APawn* ActorPawn = Cast<APawn>(this->CompContext.CharacterActor);
	if (!ActorPawn || !this->CompContext.CharacterActor->Implements<UControllable>()) {
		UE_LOG(LogTemp, Error, TEXT("ControlSystem Initiation Failure (Owner must be a pawn) & Implements IControllable"), *GetNameSafe(this)); return;
	}
	this->ControlSystemSetup(ActorPawn->GetController());
}

void UControlSystem::ControlSystemSetup(AController* NewController)
{
	if (!NewController->IsPlayerController() || !this->GetOwner()->InputComponent) return;
	UInputComponent* InputComp = this->GetOwner()->InputComponent;
	InputComp->BindAction("Interact", IE_Pressed, this, &UControlSystem::ControlInteract);
	InputComp->BindAction("Attack", IE_Pressed, this, &UControlSystem::ControlAttack);
	InputComp->BindAction("HeavyAttack", IE_Pressed, this, &UControlSystem::ControlHeavyAttackCharge);
	InputComp->BindAction("HeavyAttack", IE_Released, this, &UControlSystem::ControlHeavyAttackRelease);
	InputComp->BindAction("Dodge", IE_Pressed, this, &UControlSystem::ControlDodge);
	InputComp->BindAction("ToggleAmpField", IE_Pressed, this, &UControlSystem::ControlToggleAmpField);
	InputComp->BindAction("VentAmp", IE_Pressed, this, &UControlSystem::ControlVentAmp);
	InputComp->BindAction("UseItem", IE_Pressed, this, &UControlSystem::ControlUseItem);
	InputComp->BindAxis("MoveForward", this, &UControlSystem::ControlMoveForward);
	InputComp->BindAxis("MoveRight", this, &UControlSystem::ControlMoveRight);
	InputComp->BindAxis("CycleItem", this, &UControlSystem::ControlCycleItem);
}

void UControlSystem::UpdateWorldSpaceVectors() {
	float YawRotation = IControllable::Execute_GetControlledCameraYawRotation(this->CompContext.CharacterActor);
	FRotator TempRotator = FRotator(0, YawRotation, 0);
	this->CompContext.Store->MovementModule.WorldSpaceTargetDir = TempRotator.RotateVector(this->RawInput);
}

void UControlSystem::ControlSystemDisable(AController* OldController)
{
	if (!OldController->IsPlayerController() || !this->GetOwner()->InputComponent) return;
	UInputComponent* InputComp = this->GetOwner()->InputComponent;
	InputComp->RemoveActionBinding("Interact", IE_Pressed);
	InputComp->RemoveActionBinding("Attack", IE_Pressed);
	InputComp->RemoveActionBinding("HeavyAttack", IE_Pressed);
	InputComp->RemoveActionBinding("HeavyAttack", IE_Released);
	InputComp->RemoveActionBinding("Dodge", IE_Pressed);
	InputComp->RemoveActionBinding("ToggleAmpField", IE_Pressed);
	InputComp->RemoveActionBinding("VentAmp", IE_Pressed);
	InputComp->RemoveActionBinding("UseItem", IE_Pressed);
	InputComp->AxisBindings.Empty();
}

void UControlSystem::ControlSystemPossess(AActor* PossessInstigator)
{
	APawn* OwnerPawn = Cast<APawn>(this->CompContext.CharacterActor);
	APawn* InstigatorPawn = Cast<APawn>(PossessInstigator);
	if (!OwnerPawn 
		|| !InstigatorPawn 
		|| !InstigatorPawn->GetController()  
		|| !InstigatorPawn->GetController()->IsPlayerController()
		|| !InstigatorPawn->Implements<UContextAvailable>()
	) return;
	AController* PlayerController = InstigatorPawn->GetController();
	PlayerController->UnPossess();
	
	FCharacterContext InstigatorCtx;
	IContextAvailable::Execute_GetContext(InstigatorPawn, InstigatorCtx);

	if (!InstigatorCtx.AIController) {
		InstigatorPawn->SpawnDefaultController();
		AAIController* InstigatorAIController = Cast<AAIController>(InstigatorPawn->GetController());
		if (InstigatorAIController) {
			InstigatorCtx.AIController = InstigatorAIController;
		}
	}
	else {
		InstigatorCtx.AIController->Possess(InstigatorPawn);
	}

	PlayerController->Possess(OwnerPawn);

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
	this->CompContext.EventBus->StateAxisDelegate.Broadcast(EActionList::ActionMoveForward, Value);
}

void UControlSystem::ControlMoveRight(float Value)
{
	this->RawInput.Y = Value;
	this->RawInput.Normalize();
	this->UpdateWorldSpaceVectors();
	this->CompContext.EventBus->StateAxisDelegate.Broadcast(EActionList::ActionMoveRight, Value);
}

void UControlSystem::ControlAttack()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Pressed);
}

void UControlSystem::ControlHeavyAttackCharge()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionHeavyAttack, IE_Pressed);
}

void UControlSystem::ControlHeavyAttackRelease()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionHeavyAttack, IE_Pressed);
}

void UControlSystem::ControlToggleAmpField()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionToggleAmpField, IE_Pressed);
}

void UControlSystem::ControlUseItem()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionUseItem, IE_Pressed);
}

void UControlSystem::ControlVentAmp()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionVentAmp, IE_Pressed);
}

void UControlSystem::ControlDodge()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionDodge, IE_Pressed);
}

void UControlSystem::ControlInteract()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionInteract, IE_Pressed);
}
