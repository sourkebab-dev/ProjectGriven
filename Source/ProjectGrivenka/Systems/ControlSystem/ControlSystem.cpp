// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlSystem.h"
#include "AIController.h"
#include "Controllable.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/ContextUtilities/ContextStore.h"
#include "ProjectGrivenka/VectorMathLib.h"

UControlSystem::UControlSystem() : UBaseContextableComponent() {
	this->PrimaryComponentTick.bCanEverTick = true;
}

void UControlSystem::Init()
{
	Super::Init();
	if (!this->CompContext.EventBus) {
		UE_LOG(LogTemp, Error, TEXT("Eventbus Initiation Failure"), *GetNameSafe(this)); return;
	}
	this->CompContext.EventBus->PossessionDelegate.AddDynamic(this, &UControlSystem::ControlSystemSetup);
	this->CompContext.EventBus->UnpossesionDelegate.AddDynamic(this, &UControlSystem::ControlSystemDisable);
	this->CompContext.EventBus->AnimDelegate.AddDynamic(this, &UControlSystem::AnimHandler);
	APawn* ActorPawn = Cast<APawn>(this->CompContext.CharacterActor);
	if (!ActorPawn || !this->CompContext.CharacterActor->Implements<UControllable>()) {
		UE_LOG(LogTemp, Error, TEXT("ControlSystem Initiation Failure (Owner must be a pawn) & Implements IControllable"), *GetNameSafe(this)); return;
	}
	this->ControlSystemSetup(ActorPawn->GetController());
}

void UControlSystem::ControlSystemSetup(AController* NewController)
{	
	if (!NewController || !NewController->IsPlayerController() || (!this->GetOwner()->InputComponent && !NewController->InputComponent)) {
		this->SetComponentTickEnabled(false);
		return;
	}
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
	float YawRotation = IControllable::Execute_GetControlledCameraYawRotation(this->CompContext.CharacterActor);
	FRotator TempRotator = FRotator(0, YawRotation, 0);
	this->CompContext.Store->MovementModule.WorldSpaceTargetDir = TempRotator.RotateVector(this->RawInput);
}

void UControlSystem::ControlSystemDisable(AController* OldController)
{
	if (!OldController || !OldController->IsPlayerController() || (!this->GetOwner()->InputComponent && !OldController->InputComponent)) return;
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
	InputComp->AxisBindings.Empty();
	this->SetComponentTickEnabled(false);
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
		//Note: disabled cause controller is already set in BaseAIController OnPossess
		//AAIController* InstigatorAIController = Cast<AAIController>(InstigatorPawn->GetController());
		//if (InstigatorAIController) {
		//	InstigatorCtx.AIController = InstigatorAIController;
		//}
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

void UControlSystem::ControlAttackRelease()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionAttack, IE_Released);
}

void UControlSystem::ControlBlock()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionBlock, IE_Pressed);
}

void UControlSystem::ControlBlockRelease()
{
	this->CompContext.EventBus->StateActionDelegate.Broadcast(EActionList::ActionBlock, IE_Released);
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
