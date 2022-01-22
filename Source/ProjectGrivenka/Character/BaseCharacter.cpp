// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "ProjectGrivenka/AI/BaseAIController.h"
#include "ProjectGrivenka/Interfaces/Interactable.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Movement = FVector::ZeroVector;
	//Sponge: Interactioncollider need to be set so that it's ignored by damage collider
	this->InteractionCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Collider"));
	this->InteractionCollider->SetupAttachment(RootComponent);
	this->InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::SaveInteractable);
	this->InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::RemoveInteractable);
	this->RotationRate = 20;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

#pragma region Life Cycles
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//sponge: might need to move this to controlsys
	this->RotateToTarget();
}
#pragma endregion

void ABaseCharacter::SaveInteractable(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//SPONGE: need to check if player is looking at item
	if (!OtherActor->Implements<UInteractable>() || !IInteractable::Execute_CheckIsInteractable(OtherActor, this) || OtherActor == this) return;
	this->InteractableItem = OtherActor;
	GLog->Log("Overlap interaction");
}

void ABaseCharacter::RemoveInteractable(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (!OtherActor->Implements<UInteractable>()) return;
	if (OtherActor == this->InteractableItem) {
		this->InteractableItem = nullptr;
		GLog->Log("Remove interaction");
	}
}

void ABaseCharacter::Interact_Implementation(AActor* InteractInstigator)
{
	//this->GetDialogueSystemComp()->StartDefaultDialogue();
}

bool ABaseCharacter::CheckIsInteractable_Implementation(AActor* InteractInstigator)
{
	return true;
}

void ABaseCharacter::RequestInteractionInfo_Implementation(AActor* InteractInstigator, FInteractionInfo& OutInteractionInfo)
{
	OutInteractionInfo.IsLatent = false;
	OutInteractionInfo.LocomotionType = EInteractablesLocomotionType::INL_BLOCK;
	OutInteractionInfo.UsageType = EInteractablesUsageType::INU_ONEOFF;
}

void ABaseCharacter::SaveData_Implementation()
{
}

void ABaseCharacter::LoadData_Implementation()
{
	/*
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	if (!GameInstance || !AssetsData) return;


	//Appearance Load
	FCommonSkeletalMesh* SKMeshStruct = AssetsData->SKMeshData->FindRow<FCommonSkeletalMesh>(CharacterData.Appearance.SkeletalMeshId, "");
	FCommonMaterial* MaterialStruct = AssetsData->MaterialData->FindRow<FCommonMaterial>(CharacterData.Appearance.MaterialId, "");
	if (!this->GetMesh()) { GLog->Log("NOMESH"); return; }
	if (!SKMeshStruct) { GLog->Log("nomesk found"); return; }

	this->GetMesh()->SetSkeletalMesh(SKMeshStruct->Mesh);
	this->GetMesh()->SetMaterial(0, MaterialStruct->Material.MaterialInterface);


	//Anim Load
	this->GetMesh()->SetAnimInstanceClass(CharacterData.Appearance.AnimClass);

	//Attributes Load
	//this->StatesComp->CompContext->CharacterSystemComp->InitializeAttributes(CharacterData.Attributes);
	//this->StatesComp->CompContext->EquipmentSystemComp->LoadEquipments(CharacterData);


	//AI Load
	//SPONGE: MIGHT NEED TO PUT ON BASENPC INSTEAD
	this->SpawnDefaultController();
	*/
}

#pragma region Character Functional Shit

void ABaseCharacter::RotateToTarget()
{
	if (!this->RotateTarget) return;
	FRotator CurrentRotation = GetActorRotation();
	float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	FVector CurrentLocation = GetActorLocation();
	FVector RotateTargetLocation = RotateTarget->GetActorLocation();
	FRotator RotationTarget = FRotator::ZeroRotator;
	RotationTarget.Yaw = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, RotateTargetLocation).Yaw;
	FRotator InterpRotation = FMath::RInterpTo(CurrentRotation, RotationTarget, DeltaSeconds, this->RotationRate);
	SetActorRotation(InterpRotation);
}
#pragma endregion

#pragma region AI Functionalities
void ABaseCharacter::AISetAggroTargetFromList()
{
	if (!this->AggroList.Num()) return;
	this->AISetAggroTarget(this->AggroList[0]);
}

void ABaseCharacter::AISetAggroTarget(ABaseCharacter* NewAggroTarget) {
	this->AggroTarget = NewAggroTarget;
	this->RotateTarget = NewAggroTarget;
	//this->AIController->SetBBAggroTarget(NewAggroTarget);
}

void ABaseCharacter::AISet(ABaseAIController* NewController) {
	this->AIController = NewController;
}
void ABaseCharacter::AIRemove() {
	this->AIController = nullptr;
}
#pragma endregion

#pragma region State Management
/*
void ABaseCharacter::ActionInteract() {
	if (!this->InteractableItem) return;
	//SPONGE: probably not a good idea?
	this->InteractableItem->Execute_Interact(Cast<UObject>(this->InteractableItem), this);
}
*/
#pragma endregion

