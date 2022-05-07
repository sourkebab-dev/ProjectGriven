// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Equipments/BaseEquipment.h"
#include "Animation/AnimMontage.h"
#include "ProjectGrivenka/GlobalDefinitions.h"
#include "ProjectGrivenka/Equipments/EquipmentDefinitions.h"
#include "BaseWeapon.generated.h"

//SPONGE: equipments need to have something that stores its initter which matches the fpersisted values

UCLASS()
class PROJECTGRIVENKA_API ABaseWeapon : public ABaseEquipment
{
	GENERATED_BODY()

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:

	TArray<AActor*> Hitlist;

	//SPONGE: Might change to something like character system
	//sponge: Might need to use fweaponinfo instead of each attributes like this
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FortitudeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RawDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RawStaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ElementalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RawDamageAbsorption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EDamageElementType> ElementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* DamageCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FAttackValues> ComboItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttackValues> HeavyComboItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttackValues> LungeAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseWeapon> WeaponTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName VariantId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid EquipmentId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlockInfo BlockInfo;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DisableCollision();

public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	FORCEINLINE class UCapsuleComponent* GetDamageCollider() const { return DamageCollider; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void LoadData(FWeaponInfo InWeaponInfo);

	virtual float CalculateAttackStaminaCost(FAttackValues InCurrentAttack);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
