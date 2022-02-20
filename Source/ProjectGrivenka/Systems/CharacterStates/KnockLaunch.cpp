// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockLaunch.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveVector.h"
#include "ProjectGrivenka/VectorMathLib.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/ContextUtilities/ContextHelperLib.h"

void UKnockLaunch::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	//Sponge: might need to add recover from launch
}

void UKnockLaunch::OnStateEnter_Implementation()
{
	//LaunchUnrelated Setup
	UContextHelperLib::SetDamageCollider(this->StatesComp->CompContext, false);

	//Setup
	this->LaunchStartLocation = this->StatesComp->CompContext->CharacterActor->GetActorLocation();
	this->LaunchApexDelegate.BindUFunction(this, "OnLaunchApex");
	this->HitGroundDelegate.BindUFunction(this, "OnHitGround");
	float DotProduct = FVector::DotProduct(this->StatesComp->CompContext->CharacterActor->GetActorForwardVector(), this->StatesComp->CrossStateData.DamageInstigator->GetActorForwardVector());
	TArray<FLaunchData> LaunchType;
	//sponge: still needto facilitate launch up
	if (DotProduct < 0) {
		FRotator RotateToInstigator = FRotator(0, UKismetMathLibrary::FindLookAtRotation(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->StatesComp->CrossStateData.DamageInstigator->GetActorLocation()).Yaw, 0);
		this->StatesComp->CompContext->CharacterActor->SetActorRotation(RotateToInstigator);
		LaunchType = this->LaunchBack;
	}
	else {
		FRotator RotateToInstigator = FRotator(0, UKismetMathLibrary::FindLookAtRotation(this->StatesComp->CompContext->CharacterActor->GetActorLocation(), this->StatesComp->CrossStateData.DamageInstigator->GetActorLocation()).Yaw + 180 , 0);
		this->StatesComp->CompContext->CharacterActor->SetActorRotation(RotateToInstigator);
		LaunchType = this->LaunchFront;
		this->IsReverseCurve = true;
	}
	if (LaunchType.Num() == 0) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Pressed);
		return;
	}
	int LaunchIndex = FMath::RandRange(0, LaunchType.Num() - 1);
	this->SelectedLaunch = LaunchType[LaunchIndex];
	float StartTime, MaxTime;
	this->SelectedLaunch.LaunchCurve->GetTimeRange(StartTime, MaxTime);
	this->PeakTime = this->FindPeakTime(StartTime / PEAKSTEPPER, MaxTime / PEAKSTEPPER);
	GLog->Log("PeakTIme");
	GLog->Log(FString::SanitizeFloat(this->PeakTime));

	//Play Montage
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->SelectedLaunch.LaunchMontage);
	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UKnockLaunch::OnLaunchEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->SelectedLaunch.LaunchMontage);
	//this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->SelectedLaunch.LaunchMontage);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HitPauseTimer, FTimerDelegate::CreateLambda([&] {
		if (this->StatesComp->CrossStateData.DamageInstigator) {
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->StatesComp->CrossStateData.DamageInstigator);
			InstigatorCtx->EventBus->HitStopDelegate.Execute(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([] {}));
		}

		//freeze -> onFreezefinish
		this->StatesComp->LockAnimation(this->StatesComp->CrossStateData.DamageInfo.ImpactType, FHitStopFinishDelegate::CreateLambda([&] {
			this->IsProcessLaunch = true;
		}));
	}), 0.005, false);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::OFF_ROTATION);
}


void UKnockLaunch::Tick_Implementation(float DeltaTime)
{
	if (!this->SelectedLaunch.LaunchCurve) return;

	if (this->IsProcessLaunch) {
		this->PooledLaunchTime += DeltaTime;
		FVector AddedVectorVal = this->SelectedLaunch.LaunchCurve->GetVectorValue(this->PooledLaunchTime);
		float Degrees = UVectorMathLib::DegreesBetweenVectors(FVector::ForwardVector, this->StatesComp->CompContext->CharacterActor->GetActorForwardVector());
		AddedVectorVal = AddedVectorVal.RotateAngleAxis(Degrees, FVector::UpVector);
		if (this->IsReverseCurve) {
			AddedVectorVal = AddedVectorVal.RotateAngleAxis(180, FVector::UpVector);
		}
		FVector EndPosition = this->LaunchStartLocation + AddedVectorVal;
		this->StatesComp->CompContext->CharacterActor->SetActorLocation(EndPosition, true);

		float StartTime, MaxTime;
		this->SelectedLaunch.LaunchCurve->GetTimeRange(StartTime, MaxTime);
		if (this->PooledLaunchTime >= MaxTime) {
			this->IsProcessLaunch = false;
		}
	}

	if (this->PooledLaunchTime >= this->PeakTime && this->LaunchApexDelegate.IsBound()) {
		this->LaunchApexDelegate.Execute();
	}

	FVector PelvisLocation = this->StatesComp->CompContext->SkeletalMeshComp->GetSocketLocation("pelvis");
	TArray<AActor*> IgnoreActors;
	FHitResult PelvisHitResult;
	//Sponge: still need to add a force break if no trace were hit
	if (this->HitGroundDelegate.IsBound() && UKismetSystemLibrary::SphereTraceSingle(this->GetWorld(), PelvisLocation, PelvisLocation, 30.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::ForOneFrame, PelvisHitResult, true)) {
		this->HitGroundDelegate.Execute();
	}
}

void UKnockLaunch::OnStateExit_Implementation()
{
	UContextHelperLib::SetDamageCollider(this->StatesComp->CompContext, true);
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitPauseTimer);
	if (!this->StatesComp->CompContext->CharacterAnim) return;
	FAnimMontageInstance* AnimMontageInstance = this->StatesComp->CompContext->CharacterAnim->GetActiveInstanceForMontage(this->SelectedLaunch.LaunchMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->StatesComp->CompContext->CharacterAnim->Montage_Stop(0.25);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);

}

float UKnockLaunch::FindPeakTime(int MultiplierStart, int MultiplierEnd)
{
	float MinTR, MaxTR;
	this->SelectedLaunch.LaunchCurve->GetTimeRange(MinTR, MaxTR);

	GLog->Log("StartEnd");
	GLog->Log(FString::SanitizeFloat(MultiplierStart));
	GLog->Log(FString::SanitizeFloat(MultiplierEnd));


	int MidMultiplier = ( MultiplierStart + MultiplierEnd ) / 2;
	int PrevMultiplier = ( MultiplierStart + MultiplierEnd ) / 2 - 1;
	int NextMultiplier = ( MultiplierStart + MultiplierEnd ) / 2 + 1;
	GLog->Log("Node");
	GLog->Log(FString::SanitizeFloat(MidMultiplier));
	GLog->Log(FString::SanitizeFloat(PrevMultiplier));
	GLog->Log(FString::SanitizeFloat(NextMultiplier));


	GLog->Log("MidValue");
	GLog->Log(FString::SanitizeFloat(MidMultiplier * PEAKSTEPPER));
	GLog->Log(FString::SanitizeFloat(this->SelectedLaunch.LaunchCurve->GetVectorValue(MidMultiplier * PEAKSTEPPER).Z));
	GLog->Log("PrevValue");
	GLog->Log(FString::SanitizeFloat(PrevMultiplier * PEAKSTEPPER));
	GLog->Log(FString::SanitizeFloat(this->SelectedLaunch.LaunchCurve->GetVectorValue(PrevMultiplier * PEAKSTEPPER).Z));
	GLog->Log("NextValue");
	GLog->Log(FString::SanitizeFloat(NextMultiplier * PEAKSTEPPER));
	GLog->Log(FString::SanitizeFloat(this->SelectedLaunch.LaunchCurve->GetVectorValue(NextMultiplier * PEAKSTEPPER).Z));

	if (PrevMultiplier >=  MinTR / PEAKSTEPPER && this->SelectedLaunch.LaunchCurve->GetVectorValue(PrevMultiplier * PEAKSTEPPER).Z > this->SelectedLaunch.LaunchCurve->GetVectorValue(MidMultiplier * PEAKSTEPPER).Z) {
		GLog->Log("PrevChosen");
		return FindPeakTime(MultiplierStart, PrevMultiplier);
	}
	else if (NextMultiplier <=  MaxTR / PEAKSTEPPER && this->SelectedLaunch.LaunchCurve->GetVectorValue(NextMultiplier * PEAKSTEPPER).Z > this->SelectedLaunch.LaunchCurve->GetVectorValue(MidMultiplier * PEAKSTEPPER).Z) {
		GLog->Log("NextChosen");
		return FindPeakTime(NextMultiplier, MultiplierEnd);
	}
	else {
		GLog->Log("MidChosen");
		return MidMultiplier * PEAKSTEPPER;
	}
}

void UKnockLaunch::OnLaunchApex()
{
	GLog->Log("apex");
	this->LaunchApexDelegate.Clear();
	this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("ApexEnd", this->SelectedLaunch.LaunchMontage);
}


void UKnockLaunch::OnHitGround()
{
	this->HitGroundDelegate.Clear();
	this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("HitGround", this->SelectedLaunch.LaunchMontage);
}


void UKnockLaunch::OnLaunchEnd(UAnimMontage* Montage, bool bInterrupted)
{
	this->StatesComp->CrossStateData.DamageInstigator = nullptr;
	this->StatesComp->CrossStateData.DamageInfo = FDamageInfo();
	this->StatesComp->CrossStateData.KnockDownMontage = this->SelectedLaunch.DownMontage;
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.KnockedDown"), EActionList::ActionNone, IE_Pressed);
}
