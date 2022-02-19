// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathState.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"


void UDeathState::OnStateEnter_Implementation()
{
	Super::OnStateEnter_Implementation();
	this->StatesComp->CompContext->EventBus->DeathDelegate.Broadcast(this->StatesComp->CrossStateData.DamageInstigator, this->StatesComp->CrossStateData.DamageInfo);
	this->StatesComp->CompContext->CombatModule.IsDead = true;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	//FRotator RotateToInstigator = FRotator( 0 , UKismetMathLibrary::FindLookAtRotation(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->DeathInstigator->GetActorLocation()).Yaw, 0);
	//this->StatesComp->CompContext->CharacterActor->SetActorRotation(RotateToInstigator);
	this->StartDeathAnim();
}

void UDeathState::StartDeathAnim()
{
	bool IsHeavyAnim = this->StatesComp->CrossStateData.DamageInfo.ImpactType >= EDamageImpactType::DI_HIGH;
	float DotProduct = FVector::DotProduct(this->StatesComp->CompContext->CharacterActor->GetActorForwardVector(), this->StatesComp->CrossStateData.DamageInstigator->GetActorForwardVector());
	if (DotProduct > 0.3) {
		this->CurrentDeathMontage = IsHeavyAnim ? this->DeathBackMontage.HeavyMontage : this->DeathBackMontage.DefaultMontage;
	}
	else if (DotProduct > -0.3 && DotProduct < 0.3) {
		FRotator RotateToInstigator = FRotator( 0 , UKismetMathLibrary::FindLookAtRotation(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->StatesComp->CrossStateData.DamageInstigator->GetActorLocation()).Yaw, 0);
		this->StatesComp->CompContext->CharacterActor->SetActorRotation(RotateToInstigator);
		this->CurrentDeathMontage = IsHeavyAnim ? this->DeathFrontMontage.HeavyMontage : this->DeathFrontMontage.DefaultMontage;
	}
	else {
		this->CurrentDeathMontage = IsHeavyAnim ? this->DeathFrontMontage.HeavyMontage : this->DeathFrontMontage.DefaultMontage;
	}

	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->CurrentDeathMontage);

	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, FTimerDelegate::CreateLambda([&] {
		this->StatesComp->CrossStateData.DamageInfo.ImpactType = EDamageImpactType::DI_HIGH;
		if (this->StatesComp->CrossStateData.DamageInstigator) {
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->StatesComp->CrossStateData.DamageInstigator);
			InstigatorCtx->EventBus->HitStopDelegate.Execute(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([] {}));
		}

		//freeze -> onFreezefinish
		this->StatesComp->LockAnimation(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([&] {}));
	}), 0.005, false);


	FTimerHandle HandleTmr;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(HandleTmr, FTimerDelegate::CreateLambda([&] {
		if (!this->StatesComp || !this->StatesComp->CompContext || !this->StatesComp->CompContext->SkeletalMeshComp) return;
		if (Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)) {
			Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		};
		this->StatesComp->CompContext->SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->StatesComp->CompContext->SkeletalMeshComp->SetSimulatePhysics(true);
		this->StatesComp->CrossStateData.DamageInstigator = nullptr;
		this->StatesComp->CrossStateData.DamageInfo = FDamageInfo();
	}), this->CurrentDeathMontage->GetPlayLength() - 0.5f, false);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::DEATH);
}
