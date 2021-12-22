// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectGrivenka/Character/ControllableCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystem.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventory.h"
#include "ProjectGrivenka/Equipments/Weapons/BaseWeapon.h"
#include "ProjectGrivenka/AI/BaseAIController.h"
#include "ProjectGrivenka/Utilities/BaseGameInstance.h"
#include "ProjectGrivenka/Utilities/UIManager.h"

AControllableCharacter::AControllableCharacter() : ABaseCharacter() {
	MouseTarget = CreateDefaultSubobject<UChildActorComponent>(TEXT("MouseTarget"));
	MouseTarget->SetupAttachment(RootComponent);

	CameraBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBase"));
	CameraBase->SetupAttachment(RootComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CameraBase);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	// Something
	CursorWorldLocation = FVector::ZeroVector;
	IsLockedOn = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AControllableCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AControllableCharacter::ActionInteract);
	//PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AControllableCharacter::ActionAttack);
	//PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AControllableCharacter::ActionHeavyAttackCharge);
	//PlayerInputComponent->BindAction("HeavyAttack", IE_Released, this, &AControllableCharacter::ActionHeavyAttackRelease);
	//PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AControllableCharacter::ActionDodge);
	//PlayerInputComponent->BindAction("ToggleAmpField", IE_Pressed, this, &AControllableCharacter::ActionToggleAmpField);
	//PlayerInputComponent->BindAction("VentAmp", IE_Pressed, this, &AControllableCharacter::ActionVentAmp);
	//PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AControllableCharacter::ActionUseItem);
	//PlayerInputComponent->BindAxis("MoveForward", this, &AControllableCharacter::ActionMoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AControllableCharacter::ActionMoveRight);
	//PlayerInputComponent->BindAxis("CycleItem", this, &AControllableCharacter::ActionCycleItem);
}



void AControllableCharacter::BeginPlay()
{
	Super::BeginPlay();
	this->MyPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return;
	//SPONGE: i should probably put this on other place (maybe everytime this gets controlled by a player controller)
	GameInstance->UIManager->EquipmentChangeDelegate.AddDynamic(this, &AControllableCharacter::OnUIEquipmentChange);
}

void AControllableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (this->IsPlayerControlled()) {
		this->RotateTarget = MouseTarget->GetChildActor();
		this->MouseTarget->SetHiddenInGame(false, true);
	}
	else {
		if (this->AIController) this->AIController->BTStart();
		//SPONGE: Maybe put cursor in gameinstance?
		this->MouseTarget->SetHiddenInGame(true, true);
		this->RotateTarget = this->AggroTarget ? this->AggroTarget : nullptr;
	}
}

void AControllableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Sponge: maybe theres a better way
	this->MoveCursor();
}

void AControllableCharacter::MoveCursor() {
	if (this->IsLockedOn) {

	}
	else {
		FHitResult MouseHit;
		this->MyPlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, false, MouseHit);
		this->CursorWorldLocation = MouseHit.Location;
	}

	this->MouseTarget->SetWorldLocation(this->CursorWorldLocation);
}


/*
void AControllableCharacter::ActionCycleItem(float Value)
{
	int IndexModifier = (int)Value;
	
	//this->CharacterContext.InventorySystemComp->SetSelectedItem(this->CharacterContext.InventorySystemComp->SelectedItemBeltIdx + IndexModifier);
}
*/

void AControllableCharacter::SaveData_Implementation()
{
	//SPONGE::REFACTOR: MIGHT NEED TO MOVE THIS SOMEWHERE ELSE 
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!GameInstance) return;

	int CharIndex = -1;
	for (int i = 0; i < GameInstance->Crew.Num(); i++) {
		if (GameInstance->Crew[i].Info.CharacterId == this->CharacterId) {
			CharIndex = i;
			break;
		}
	}
	GLog->Log(FString::FromInt(CharIndex));
	if (CharIndex == -1) return;

//	if (!this->CharacterContext.EquipmentSystemComp->GetEquippedWeapon()) return;
	//GameInstance->Crew[CharIndex].Equipments.WeaponInfo.BaseClass = this->CharacterContext.EquipmentSystemComp->GetEquippedWeapon()->WeaponTypeClass;
	//GameInstance->Crew[CharIndex].Equipments.WeaponInfo.VariantId = this->CharacterContext.EquipmentSystemComp->GetEquippedWeapon()->VariantId;
}

void AControllableCharacter::LoadData_Implementation(FPersisted_CharacterCompleteData CharacterData)
{
	Super::LoadData_Implementation(CharacterData);
	this->CharacterId = CharacterData.Info.CharacterId;
}

void AControllableCharacter::PossessCharacter()
{
	//sponge: we can move this to controlsystem i think ? 
	AControllableCharacter* InstigatorCC = Cast<AControllableCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (!InstigatorCC || !PlayerController || !GameInstance) return;
	PlayerController->UnPossess();
	if (!InstigatorCC->AIController) InstigatorCC->SpawnDefaultController();
	else InstigatorCC->AIController->Possess(InstigatorCC);
	PlayerController->Possess(this);
	GameInstance->SetControlledCrewId(this->CharacterId);
}

void AControllableCharacter::OnUIEquipmentChange(FGuid InCharGuid, FPersisted_EquipmentInfo InEquipmentInfo)
{
	if (!this->CharacterId.IsValid() || InCharGuid != this->CharacterId) return;
	FPersisted_CharacterCompleteData CharData = FPersisted_CharacterCompleteData();
	CharData.Equipments.WeaponInfo = InEquipmentInfo;
	//this->CharacterContext.EquipmentSystemComp->LoadEquipments(CharData);
;}
