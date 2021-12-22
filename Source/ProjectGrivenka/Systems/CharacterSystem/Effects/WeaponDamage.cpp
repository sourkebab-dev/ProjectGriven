// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDamage.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"



void UWeaponDamage::Init(AActor* NewEffectInstigator, AActor* NewEffectReceiver, FEffectInfo InEffectInfo) {
	UGrivenkaDataSingleton* AssetsData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	UEffectPrefab* EffectPrefab = AssetsData->EffectPrefab->EffectAssets.FindRef("Util_WeaponHit");
	if (!EffectPrefab) return;
	FEffectInfo TempEffectInfo = EffectPrefab->EffectInfo;
	Super::Init(NewEffectInstigator, NewEffectReceiver, TempEffectInfo);
}

void UWeaponDamage::OnExecuteEffect() {
	//if (this->EffectReceiverCharacter && this->EffectReceiverCharacter->CurrentStateCode == ECharacterStates::StateDodge) return;
	
	 

	UCharacterSystem* InstigatorComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectInstigator);
	UCharacterSystem* ReceiverComp = ICharacterSystemAvailable::Execute_GetCharacterSystemComp(this->EffectReceiver);

	float WeaponDamage = InstigatorComp->GetAttributeCurrentValue(EAttributeCode::ATT_WeaponDamage);
	float MovingValues = InstigatorComp->GetAttributeCurrentValue(EAttributeCode::ATT_WeaponMovingValues);

	float ReceiverDefense = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Defense);
	float ReceiverHealth = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Health);
	float ReceiverFortude = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Fortitude);
	float ReceiverAmp = ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Amp);
	

	float DamageByMovement = WeaponDamage * (MovingValues / 100);

	GLog->Log("WeaponDamage");
	GLog->Log(FString::SanitizeFloat(WeaponDamage));
	GLog->Log("DamageBymove");
	GLog->Log(FString::SanitizeFloat(DamageByMovement));
	GLog->Log("ReceiverDefense");
	GLog->Log(FString::SanitizeFloat(ReceiverDefense));
	
	//sponge: might need to change the way amp is used
	//if (ReceiverComp->CombatFlags.IsAmpActivated) {
	//	float AmpDamage = DamageByMovement * 0.50;
	//	ReceiverComp->Attributes->SetAmp(ReceiverAmp + AmpDamage * (AmpDamage / (AmpDamage + ReceiverDefense)));
	//	if (ReceiverComp->Attributes->GetAmp() >= ReceiverComp->Attributes->GetMaxAmp()) {
			//SPONGE: Should probably add amp overload state
	//		ReceiverComp->Attributes->SetFortitude(0);
	//		ReceiverComp->Attributes->SetAmp(0);
	//		ReceiverComp->CombatFlags.IsAmpActivated = false;
	//	}
	//}
	//else {
		ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Health, ReceiverHealth - DamageByMovement * (DamageByMovement / (DamageByMovement + ReceiverDefense)));
		ReceiverComp->SetAttributeValue(EAttributeCode::ATT_Fortitude, ReceiverFortude - (MovingValues / 50));
		GLog->Log("ReceiverHealth");
		GLog->Log(FString::SanitizeFloat(ReceiverComp->GetAttributeCurrentValue(EAttributeCode::ATT_Health)));
	//}
	

	//Sponge: need to use event for this
	//if (this->EffectReceiverCharacter) {
		//this->EffectReceiverCharacter->SetupHitReaction(InstigatorComp->CompContext.CharacterActor, ReceiverFortude, ReceiverComp->Attributes->GetFortitude(), ReceiverComp->Attributes->GetMaxFortitude());
	//}
}

/*
void UWeaponDamage::SetupHitReaction(AActor* InstigatorActor, float OldFortitude, float NewFortitude, float MaxFortitude)
{
	//SPONGE: might change to pooled damage difference
	//SPONGE: might move this to knockback state
	float FortDamagePercentage = (OldFortitude - NewFortitude) / MaxFortitude;
	if (FortDamagePercentage > .3) {
		if (this->Execute_GetCharacterSystemComp()->Attributes->GetFortitude() <= 0.0) {
			this->ChangeState(this->CurrentState, ECharacterStates::StateKnocked, EActionList::ActionHeavyKnocked, IE_Released);
		}
		else {
			this->ChangeState(this->CurrentState, ECharacterStates::StateKnocked, EActionList::ActionMediumKnocked, IE_Released);
		}
	}
	else {
		if (this->Execute_GetCharacterSystemComp()->Attributes->GetFortitude() / this->Execute_GetCharacterSystemComp()->Attributes->GetMaxFortitude() <= .5) {
			this->ChangeState(this->CurrentState, ECharacterStates::StateKnocked, EActionList::ActionLowKnocked, IE_Released);
		}
		FVector PushDirection = this->GetActorLocation() - InstigatorActor->GetActorLocation();
		PushDirection.Normalize();
		this->TempActorLocation = this->GetActorLocation();
		this->TempTargetLocation = this->TempActorLocation + PushDirection * 50;
		this->TempPooledTime = 0.0;
		this->GetWorldTimerManager().SetTimer(this->CombatCharTimer, this, &AProjectGrivenkaCharacter::OnHitReaction, this->GetWorld()->GetDeltaSeconds(), true, -1.0f);
	}

	if (!this->IsPlayerControlled()) {
		AProjectGrivenkaCharacter* Attacker = Cast<AProjectGrivenkaCharacter>(InstigatorActor);
		if (!Attacker || this->AggroList.Contains(InstigatorActor)) return;
		this->AggroList.Add(Attacker);
		this->AISetAggroTargetFromList();
	}
}

void UWeaponDamage::OnHitReaction() {
	this->TempPooledTime += this->GetWorld()->GetDeltaSeconds();
	this->SetActorLocation(FMath::Lerp(this->TempActorLocation, this->TempTargetLocation, this->TempPooledTime / 0.1), true);
	if (FVector::Distance(this->GetActorLocation(), this->TempTargetLocation) <= 2.f || this->TempPooledTime >= 0.1) {
		this->GetWorldTimerManager().ClearTimer(this->CombatCharTimer);
	}
}
*/