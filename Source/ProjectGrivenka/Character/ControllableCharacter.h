// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGrivenka/Character/BaseCharacter.h"
#include "ControllableCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API AControllableCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AControllableCharacter();
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CameraBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* MouseTarget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector CursorWorldLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector RawInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool IsLockedOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ID")
	FGuid CharacterId;

protected:
	class APlayerController* MyPlayerController;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class UStaticMeshComponent* GetCameraBase() const { return CameraBase; }

	FORCEINLINE class UChildActorComponent* GetMouseTarget() const { return MouseTarget; }

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void SaveData_Implementation() override;
	virtual void LoadData_Implementation() override;
	
	UFUNCTION(BlueprintCallable)
	void PossessCharacter();
#pragma region Event handlers
	//virtual void OnUIEquipmentChange(FGuid InCharGuid, FPersisted_EquipmentInfo InEquipmentInfo);
#pragma endregion

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void UpdateWorldSpaceVectors();
	void MoveCursor();


};
