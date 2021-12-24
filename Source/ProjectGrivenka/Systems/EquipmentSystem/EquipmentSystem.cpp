// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSystem.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Equipments/Weapons/BaseWeapon.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"

void UEquipmentSystem::Init()
{
	Super::Init();
}

void UEquipmentSystem::DisableWeaponDamageColliders()
{
	this->WeaponR->DisableCollision();
}


void UEquipmentSystem::LoadEquipments(FPersistedEquipments InPersistedEquipments)
{
	//Weapon Load
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UWeaponPrefabs* WeaponPrefab = AssetsData->WeaponPrefabs->WeaponAssets.FindRef(InPersistedEquipments.WeaponInfo.VariantId);
	if (WeaponPrefab) {
		TSubclassOf<ABaseEquipment> WeaponClass = WeaponPrefab->WeaponInfo.GeneralInfo.EquipmentBaseClass;
		if (WeaponClass && WeaponClass->IsChildOf(ABaseWeapon::StaticClass())) {
			if (this->WeaponR) {
				if (this->CompContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
					ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->CompContext.CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Weapon"));
				}
				this->WeaponR->Destroy();
			}

			FActorSpawnParameters SpawnParams = FActorSpawnParameters();
			SpawnParams.Owner = this->GetOwner();
			ABaseWeapon* Weapon = this->GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, SpawnParams);
			FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			Weapon->AttachToComponent(this->CompContext.SkeletalMeshComp, TransformRules, "WeaponSlot");
			this->WeaponR = Weapon;
			this->WeaponR->LoadData(WeaponPrefab->WeaponInfo);

			if (this->CompContext.CharacterActor->Implements<UCharacterSystemAvailable>()) {
				ICharacterSystemAvailable::Execute_InitEffectByPrefabName(this->CompContext.CharacterActor, this->CompContext.CharacterActor, "Util_WeaponDamage", Weapon->RawDamage, true);
			}
		}
	}


}

float UEquipmentSystem::CalculateAttackStaminaCost(FAttackValues InCurrentAttack)
{
	if (!this->WeaponR) return 0;
	return this->WeaponR->CalculateAttackStaminaCost(InCurrentAttack);
}

FAttackValues UEquipmentSystem::GetNextAttackValue(TEnumAsByte<EAttackMovementType> InMovementType, FAttackValues InCurrentAttack)
{
	//sponge: need to check for unarmed later
	if (!this->WeaponR) return FAttackValues();
	TArray<FAttackValues> AttackList;
	int AtkIndex;
	switch (InMovementType) {
		case EAttackMovementType::AM_DEFAULT:
			AttackList = this->WeaponR->ComboItems;
			AtkIndex = InCurrentAttack.NextComboPointer;
			break;
		case EAttackMovementType::AM_HEAVY:
			AttackList = this->WeaponR->HeavyComboItems;
			AtkIndex = InCurrentAttack.NextHeavyPointer;
			break;
		case EAttackMovementType::AM_CHARGED:
			AttackList = this->WeaponR->HeavyComboItems;
			AtkIndex = InCurrentAttack.NextHeavyPointer;
			break;
		case EAttackMovementType::AM_LUNGE:
			AttackList = this->WeaponR->LungeAttacks;
			AtkIndex = 0;
			break;
	}

	if (AtkIndex > AttackList.Num() - 1) {
		//Sponge: should probably check back on attack state to see if attackpointer is correct
		UE_LOG(LogTemp, Error, TEXT("Invalid Attack Pointer"), *GetNameSafe(this));
		//this->StatesComp->ChangeState(this, ECharacterStates::StateDefault, EActionList::ActionNone, IE_Released);
		return FAttackValues();
	}

	if (!AttackList.Num()) {
		//this->StatesComp->ChangeState(this, ECharacterStates::StateDefault, EActionList::ActionNone, IE_Released);
		return FAttackValues();
	}

	return AttackList[AtkIndex];
}
