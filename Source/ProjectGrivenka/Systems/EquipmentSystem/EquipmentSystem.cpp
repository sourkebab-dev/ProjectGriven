// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentSystem.h"
#include "GameplayTagContainer.h"
#include "ProjectGrivenka/Equipments/Weapons/BaseWeapon.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void UEquipmentSystem::Init()
{
	Super::Init();
	//Sponge: should bind this to a function that calls weapon disabled pon currently used wapon 
	FContextSimpleDelegate SimpleDelegate;
	this->CompContext.EventBus->AddSubscriber(SimpleDelegate, EContextDelegates::CDL_WEAPON_COLLIDE_DISABLE);
}


void UEquipmentSystem::LoadEquipments(FPersisted_CharacterCompleteData CharacterData)
{
	//Weapon Load
	if (CharacterData.Equipments.WeaponInfo.BaseClass) {
		if (!CharacterData.Equipments.WeaponInfo.BaseClass->IsChildOf(ABaseWeapon::StaticClass())) {
			GLog->Log("Weapon Class is Invalid");
			return;
		}

		//remove equipped weapon
		if (this->WeaponR) {
			this->CompContext.EventBus->EffectRemoveDelegate.Broadcast(FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Equipment.Weapon"));
			this->WeaponR->Destroy();
		}


		FActorSpawnParameters SpawnParams = FActorSpawnParameters();
		SpawnParams.Owner = this->GetOwner();
		ABaseWeapon* Weapon = this->GetWorld()->SpawnActor<ABaseWeapon>(CharacterData.Equipments.WeaponInfo.BaseClass, SpawnParams);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
		Weapon->AttachToComponent(this->CompContext.SkeletalMeshComp, TransformRules, "WeaponSlot");
		this->WeaponR = Weapon;
		this->WeaponR->LoadData_Implementation(CharacterData);



		//Add to Weapon Damage Stat
		FRPGEffectInitDelegate* EffectInitDelegate = this->CompContext.EventBus->EffectApplyObservers.Find(EEffectDelegates::EDL_WEAPON_DAMAGE);
		if (EffectInitDelegate) EffectInitDelegate->Broadcast(Weapon->RawDamage, this->CompContext.CharacterActor, this->CompContext.CharacterActor);
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
