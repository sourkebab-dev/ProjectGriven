// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageColliders.h"
#include "ProjectGrivenka/Equipments/Weapons/BaseWeapon.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystem.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"

void UDamageColliders::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (Mesh->GetOwner()->Implements<UEquipmentSystemAvailable>()) {
		ABaseWeapon* EquippedWeapon = IEquipmentSystemAvailable::Execute_GetEquipmentSystemComp(Mesh->GetOwner())->GetEquippedWeapon();
		if (EquippedWeapon) {
			EquippedWeapon->ActivateCollision();
		}
	}
}

void UDamageColliders::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation)
{
	if (Mesh->GetOwner()->Implements<UEquipmentSystemAvailable>()) {
		ABaseWeapon* EquippedWeapon = IEquipmentSystemAvailable::Execute_GetEquipmentSystemComp(Mesh->GetOwner())->GetEquippedWeapon();
		if (EquippedWeapon) {
			EquippedWeapon->DisableCollision();
		}
	}
}
