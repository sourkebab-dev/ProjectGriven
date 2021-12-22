// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackState.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/EquipmentSystem/EquipmentSystemAvailable.h"

void UAttackState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
}

//SPONGE: SHOULD ADD OBJ MEMBER ACCESS VALIDATIONS
//SPONGE: NEED TO MOVE CURRENT ATTACK BACK TO ATTACK STATE
bool UAttackState::StateValidation_Implementation() {
	return this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>()
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor, EAttributeCode::ATT_Stamina) > 0 : true;
}

void UAttackState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{	
	bool IsStaminaAllowed = this->CharacterContext.CharacterActor->Implements<UCharacterSystemAvailable>() 
		? ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CharacterContext.CharacterActor,EAttributeCode::ATT_Stamina) > 0 : true;

	if (Action == EActionList::ActionAttack && EventType == IE_Pressed && !this->isHolding && this->StatesComp->CrossStateData.IsComboActive && IsStaminaAllowed) {
		this->StatesComp->CrossStateData.IsInterruptable = true;
		this->StatesComp->CrossStateData.IsComboActive = false;
		this->DoAttack(EAttackMovementType::AM_DEFAULT);
	}

	if (Action == EActionList::ActionHeavyAttack && EventType == IE_Pressed && IsStaminaAllowed) {
		this->StartAttackCharge();
	}

	if (Action == EActionList::ActionHeavyAttack && EventType == IE_Released && IsStaminaAllowed) {
		this->isHolding = false;
	}

	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable ) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}
}


void UAttackState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	if (NewEnterAction == EActionList::ActionHeavyAttack && NewEnterEvent == IE_Pressed) {
		this->StartAttackCharge();
	}
	else if (NewEnterAction == EActionList::ActionAttack && NewEnterEvent == IE_Pressed) {
		this->StatesComp->CrossStateData.IsInterruptable = true;
		this->StatesComp->CrossStateData.IsComboActive = false;
		this->DoAttack(EAttackMovementType::AM_DEFAULT);
	}
}

void UAttackState::OnStateExit_Implementation()
{
	//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
	this->StatesComp->CrossStateData.IsComboActive = true;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->isCharged = false;
	this->isHolding = false;
	this->PooledChargingTime = 0.0f;
	this->HeavyAttackStartSectionLength = 0.0;
	this->ChargedAttackStartSectionLength = 0.0;
	this->CharacterContext.CharacterAnim->StopAllMontages(0.25);
	this->CharacterContext.CharacterAnim->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);


	FContextSimpleDelegate* SimpleDelegate = this->CharacterContext.EventBus->Observers.Find(EContextDelegates::CDL_WEAPON_COLLIDE_DISABLE);
	if (SimpleDelegate) SimpleDelegate->Broadcast();
}

void UAttackState::DoAttack(TEnumAsByte<EAttackMovementType> AttackType) {
	

	if (AttackType == EAttackMovementType::AM_HEAVY || AttackType == EAttackMovementType::AM_CHARGED) {
		this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->ChargeTimer);
	}

	//sponge: should get from other later (maybe unarmed combat?)
	if (!this->CharacterContext.CharacterActor->Implements<UEquipmentSystemAvailable>()) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}

	FAttackValues NewAttack;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, AttackType, this->CurrentAttack, NewAttack);
	this->CharacterContext.CharacterAnim->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);


	//Change Moving Values Attribute SPONGE: maybe need to reset after exit attack state?
	float StaminaConsumption = IEquipmentSystemAvailable::Execute_GetCurrentAttackStaminaUsage(this->CharacterContext.CharacterActor, NewAttack);
	FRPGEffectInitDelegate* EffectInitDelegate = this->CharacterContext.EventBus->EffectApplyObservers.Find(EEffectDelegates::EDL_MOVING_VALUES);
	if (EffectInitDelegate) EffectInitDelegate->Broadcast(NewAttack.MovingValues, this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor);
	EffectInitDelegate = this->CharacterContext.EventBus->EffectApplyObservers.Find(EEffectDelegates::EDL_DEPLETE_STAMINA);
	if (EffectInitDelegate) EffectInitDelegate->Broadcast(StaminaConsumption, this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor);


	this->CurrentAttack = NewAttack;
	float MontageLength = this->CharacterContext.CharacterAnim->Montage_Play(this->CurrentAttack.AttackMontage);
	if (MontageLength) {
		if (AttackType == EAttackMovementType::AM_CHARGED || AttackType == EAttackMovementType::AM_HEAVY) {
			this->CharacterContext.CharacterAnim->Montage_JumpToSection("Attack", this->CurrentAttack.AttackMontage);
		}
		FOnMontageEnded EndAttackDelegate;
		EndAttackDelegate.BindUFunction(this, "OnAttackEnd");
		this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->CurrentAttack.AttackMontage);
		this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->CurrentAttack.AttackMontage);
	}
	
}

void UAttackState::OnAttackEnd(UAnimMontage* Montage, bool bInterrupted) {
	
	if (!bInterrupted) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	} else {
		//UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
		//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
	}
}


void UAttackState::StartAttackCharge(){
	
	if (!this->StatesComp->CrossStateData.IsComboActive) return;
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->StatesComp->CrossStateData.IsComboActive = false;
	this->isCharged = false;
	this->isHolding = true;
	this->PooledChargingTime = 0.0;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->ChargeTimer, this, &UAttackState::OnAttackCharging, this->CharacterContext.CharacterActor->GetWorld()->GetDeltaSeconds(), true, -1.0f);
	//Sponge: need to do somethign with isbound checks
	FAttackValues TempAtk;
	IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_HEAVY, this->CurrentAttack, TempAtk);
	this->CharacterContext.CharacterAnim->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	this->CharacterContext.CharacterAnim->Montage_Play(TempAtk.AttackMontage);
	int SecIdx = TempAtk.AttackMontage->GetSectionIndex("Default");
	int SecIdxCharge = TempAtk.AttackMontage->GetSectionIndex("Charge");
	this->HeavyAttackStartSectionLength = TempAtk.AttackMontage->GetSectionLength(SecIdx) + TempAtk.AttackMontage->GetSectionLength(SecIdxCharge);
	
}

void UAttackState::OnAttackCharging() {
	this->PooledChargingTime += this->CharacterContext.CharacterActor->GetWorld()->GetDeltaSeconds();

	//process attack based on section length (finish section before launching attack)
	if (!this->isHolding && !this->isCharged && this->PooledChargingTime >= this->HeavyAttackStartSectionLength) {
		this->DoAttack(EAttackMovementType::AM_HEAVY);
	} else if (this->PooledChargingTime >= 1.0f && !this->isCharged) {
		this->isCharged = true;
		FAttackValues TempAtk;
		IEquipmentSystemAvailable::Execute_GetNextMainAttack(this->CharacterContext.CharacterActor, EAttackMovementType::AM_CHARGED, this->CurrentAttack, TempAtk);
		this->CharacterContext.CharacterAnim->Montage_Play(TempAtk.AttackMontage);
		this->CharacterContext.CharacterAnim->Montage_JumpToSection("Start", TempAtk.AttackMontage);
		int SecIdx = TempAtk.AttackMontage->GetSectionIndex("Start");
		int SecIdxCharge = TempAtk.AttackMontage->GetSectionIndex("Charge");
		this->ChargedAttackStartSectionLength = TempAtk.AttackMontage->GetSectionLength(SecIdx) + TempAtk.AttackMontage->GetSectionLength(SecIdxCharge);
	} else if (this->isCharged && !this->isHolding && this->PooledChargingTime >= this->ChargedAttackStartSectionLength) {
		this->DoAttack(EAttackMovementType::AM_CHARGED);
	} 
}