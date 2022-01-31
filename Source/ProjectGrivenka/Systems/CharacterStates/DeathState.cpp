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

void UDeathState::Init_Implementation(UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InStatesComp);

	this->StatesComp->CompContext->EventBus->DeathDelegate.AddDynamic(this, &UDeathState::OnReceiveDeathBlow);
}

void UDeathState::OnReceiveDeathBlow(AActor* InDeathInstigator, FDamageInfo InDeathBlow)
{
	if (this->StatesComp->BlockedTags.HasAny(this->ActionTag)) return;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	this->DeathInstigator = InDeathInstigator;
	this->DeathBlow = InDeathBlow;

	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Death"), EActionList::ActionNone, IE_Released);

}

void UDeathState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->StatesComp->CompContext->CombatModule.IsDead = true;
	//FRotator RotateToInstigator = FRotator( 0 , UKismetMathLibrary::FindLookAtRotation(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->DeathInstigator->GetActorLocation()).Yaw, 0);
	//this->StatesComp->CompContext->CharacterActor->SetActorRotation(RotateToInstigator);
	this->StartDeathAnim();
}

void UDeathState::StartDeathAnim()
{
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 6, FColor::Orange, "DeathImpact");
	GEngine->AddOnScreenDebugMessage(FMath::Rand(), 6, FColor::Orange, FString::FromInt(this->DeathBlow.ImpactType));

	bool IsHeavyAnim =  this->DeathBlow.ImpactType == EDamageImpactType::DI_HIGH || this->DeathBlow.ImpactType == EDamageImpactType::DI_EXPLOSIVE;

	float DotProduct = FVector::DotProduct(this->StatesComp->CompContext->CharacterActor->GetActorForwardVector(), this->DeathInstigator->GetActorForwardVector());
	if (DotProduct > 0.3) {
		this->CurrentDeathMontage = IsHeavyAnim ? this->DeathBackMontage.HeavyMontage : this->DeathBackMontage.DefaultMontage;
	}
	else if (DotProduct > -0.3 && DotProduct < 0.3) {
		FRotator RotateToInstigator = FRotator( 0 , UKismetMathLibrary::FindLookAtRotation(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->DeathInstigator->GetActorLocation()).Yaw, 0);
		this->StatesComp->CompContext->CharacterActor->SetActorRotation(RotateToInstigator);
		this->CurrentDeathMontage = IsHeavyAnim ? this->DeathFrontMontage.HeavyMontage : this->DeathFrontMontage.DefaultMontage;
	}
	else {
		this->CurrentDeathMontage = IsHeavyAnim ? this->DeathFrontMontage.HeavyMontage : this->DeathFrontMontage.DefaultMontage;
	}

	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->CurrentDeathMontage);

	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, FTimerDelegate::CreateLambda([&] {
		this->DeathBlow.ImpactType = EDamageImpactType::DI_HIGH;
		if (this->DeathInstigator) {
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->DeathInstigator);
			InstigatorCtx->EventBus->HitStopDelegate.Execute(this->DeathBlow.ImpactType, FHitStopFinishDelegate::CreateLambda([] {}));
		}

		//freeze -> onFreezefinish
		this->StatesComp->LockAnimation(this->DeathBlow.ImpactType, FHitStopFinishDelegate::CreateLambda([&] {}));
	}), 0.005, false);


	FTimerHandle HandleTmr;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(HandleTmr, FTimerDelegate::CreateLambda([&] {
		if (!this->StatesComp || !this->StatesComp->CompContext || !this->StatesComp->CompContext->SkeletalMeshComp) return;
		if (Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)) {
			Cast<ACharacter>(this->StatesComp->CompContext->CharacterActor)->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		};
		this->StatesComp->CompContext->SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		this->StatesComp->CompContext->SkeletalMeshComp->SetSimulatePhysics(true);
	}), this->CurrentDeathMontage->GetPlayLength() - 0.5f, false);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::DEATH);
}
