// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/CapsuleComponent.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/AIContextSystem/AIContextSystemAvailable.h"
#include "ProjectGrivenka/AI/BaseAIController.h"

// Sets default values
ABaseWeapon::ABaseWeapon() : ABaseEquipment()
{

	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	this->RootComponent = RootSceneComponent;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->DamageCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Damage Collider"));
	this->DamageCollider->SetupAttachment(RootComponent);
	this->DamageCollider->SetCollisionProfileName("NoCollision");

	this->EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Equipment Mesh"));
	this->EquipmentMesh->SetupAttachment(RootComponent);
	this->EquipmentMesh->SetCollisionProfileName("NoCollision");

}

#pragma region LifeCycles
// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	this->DamageCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnWeaponOverlap);
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABaseWeapon::LoadData(FWeaponInfo InWeaponInfo)
{
	this->EquipmentMesh->SetSkeletalMesh(InWeaponInfo.GeneralInfo.EquipmentMesh);
	this->ElementType = InWeaponInfo.ElementType;
	this->ElementalDamage = InWeaponInfo.ElementalDamage;
	this->RawStaminaCost = InWeaponInfo.RawStaminaCost;
	this->RawDamage = InWeaponInfo.RawDamage;
	this->FortitudeDamage = InWeaponInfo.FortitudeDamage;
	this->WeaponTypeClass = InWeaponInfo.GeneralInfo.EquipmentBaseClass;

	if (InWeaponInfo.BlockInfo.BlockMontage) {
		this->BlockInfo.BlockMontage = InWeaponInfo.BlockInfo.BlockMontage;
	}
	if (InWeaponInfo.BlockInfo.BlockPushMontage) {
		this->BlockInfo.BlockPushMontage = InWeaponInfo.BlockInfo.BlockPushMontage;
	}

	//Sponge: Should i save variant id to weaponinfo?
	//this->VariantId = InEquipmentInfo.VariantId;
}
#pragma endregion

#pragma region Event Handlers
void ABaseWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (this->GetOwner() == OtherActor || this->Hitlist.Contains(OtherActor)) { 
		return; 
	}
	this->Hitlist.Add(OtherActor);

	if (OtherActor->Implements<UContextAvailable>()) {
		if (this->GetOwner()) {
			auto DamagableCtx = IContextAvailable::Execute_GetContext(OtherActor);
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->GetOwner());

			if (!IAIContextSystemAvailable::Execute_CheckHostility(this->GetOwner(), OtherActor)) return;

			FDamageInfo DamageInfo;
			DamageInfo.MovingValues = InstigatorCtx->CombatModule.CurrentAttack.MovingValues;
			DamageInfo.DamageDirection = InstigatorCtx->CombatModule.CurrentAttack.AttackDirection;
			DamageInfo.ImpactType = InstigatorCtx->CombatModule.CurrentAttack.ImpactType;
			DamageInfo.ElementType = this->ElementType;
			DamageInfo.RawPhysicalDamage = this->RawDamage;
			DamageInfo.RawElementalDamage = this->ElementalDamage;
			DamageInfo.IsFixed = false;
			DamagableCtx->EventBus->DamagedDelegate.Broadcast(this->GetOwner(), DamageInfo);
		}
	}

}

void ABaseWeapon::ActivateCollision() {
	this->DamageCollider->SetCollisionProfileName("OverlapAll");
}

void ABaseWeapon::DisableCollision() {
	this->DamageCollider->SetCollisionProfileName("NoCollision");
	this->Hitlist.Empty();
}

float ABaseWeapon::CalculateAttackStaminaCost(FAttackValues InCurrentAttack)
{
	return this->RawStaminaCost * (InCurrentAttack.MovingValues / 100);
}

#pragma endregion
