// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatesSystem.h"
#include "BaseState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/VectorMathLib.h"

UCharacterStatesSystem::UCharacterStatesSystem() : UBaseContextableComponent() {
	this->PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterStatesSystem::Init_Implementation()
{
	Super::Init_Implementation();
	this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	if (!this->CompContext->EventBus) return;
	this->CompContext->EventBus->StateActionDelegate.AddDynamic(this, &UCharacterStatesSystem::CurrentActionHandler);
	this->CompContext->EventBus->StateAxisDelegate.AddDynamic(this, &UCharacterStatesSystem::CurrentAxisHandler);
	this->CompContext->EventBus->AnimDelegate.AddDynamic(this, &UCharacterStatesSystem::AnimEventsHandler);
	this->CompContext->EventBus->HitDelegate.AddDynamic(this, &UCharacterStatesSystem::OnHit);
	this->CompContext->EventBus->HitStopDelegate.BindUObject(this, &UCharacterStatesSystem::LockAnimation);
	Cast<ACharacter>(this->CompContext->CharacterActor)->MovementModeChangedDelegate.AddDynamic(this, &UCharacterStatesSystem::OnMovementModeChanged);
}

void UCharacterStatesSystem::GrantAction(TSubclassOf<class UBaseState> InGrantedAction)
{
	this->GrantedActions.Add(InGrantedAction);
}

void UCharacterStatesSystem::RemoveAction(TSubclassOf<class UBaseState> InGrantedAction)
{
	this->GrantedActions.Remove(InGrantedAction);
}


void UCharacterStatesSystem::AnimEventsHandler(EAnimEvt InAnimEvent)
{
	switch (InAnimEvent) {
		case EAnimEvt::ACTIVATE_COMBO:
			this->CrossStateData.IsComboActive = true;
			break;
		case EAnimEvt::DISABLE_COMBO:
			this->CrossStateData.IsComboActive = false;
			break;
		case EAnimEvt::ACTIVATE_INTERRUPTION:
			this->CrossStateData.IsInterruptable = true;
			break;
		case EAnimEvt::DISABLE_INTERRUPTION:
			this->CrossStateData.IsInterruptable = false;
			break;
		default:
			break;
	}
}

void UCharacterStatesSystem::CurrentActionHandler(EActionList Action, EInputEvent InputEvent)
{
	if (!this->CurrentState) return;
	this->CurrentState->ActionHandler(Action, InputEvent);
}

void UCharacterStatesSystem::CurrentAxisHandler(EActionList Action, float AxisValue)
{
	if (!this->CurrentState) return;
	this->CurrentState->AxisHandler(Action, AxisValue);
}

void UCharacterStatesSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!this->CurrentState) return;
	this->CurrentState->Tick(DeltaTime);
}

bool UCharacterStatesSystem::ChangeState(FGameplayTag ChangeTo, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	if (this->BlockedTags.HasTagExact(ChangeTo)) return false;

	UBaseState* ChangeToObj = nullptr;


	if (!ChangeToObj) {
		for (TSubclassOf<UBaseState> StateClass : this->GrantedActions) {
			UBaseState* DefaultObj = Cast<UBaseState>(StateClass->GetDefaultObject());
			if (!DefaultObj) continue;
			if (DefaultObj->ActionTag.HasTag(ChangeTo)) {
				UBaseState* TempState = DuplicateObject(DefaultObj, this);
				TempState->Init(this);
				ChangeToObj = TempState;
				break;
			}
		}
	}


	if (!ChangeToObj || !ChangeToObj->StateValidation()) return false;

	FGameplayTagContainer From = FGameplayTagContainer();
	if (this->CurrentState) {
		this->CurrentState->OnStateExit();
		From = this->CurrentState->ActionTag;
	}

	this->CurrentState = ChangeToObj;
	this->BlockedTags = this->CurrentState->BlockedTag;
	this->CurrentState->OnStateEnter();

	return true;
	/*
	if (!this->IsPlayerControlled() && this->AIController) {
		this->AIController->SetBBCharacterState(ChangeTo);
	} 
	*/
}

void UCharacterStatesSystem::OnHit(AActor* HitInstigator, FDamageInfo InDamageInfo)
{
	this->CrossStateData.DamageInfo = InDamageInfo;
	this->CrossStateData.DamageInstigator = HitInstigator;

	if (this->CurrentState->ActionTag.HasTagExact(FGameplayTag::RequestGameplayTag("ActionStates.Block")) 
		&& UVectorMathLib::CheckBlockDirection(HitInstigator->GetActorLocation(), this->CompContext->CharacterActor->GetActorLocation(), this->CompContext->CharacterActor->GetActorForwardVector())) {
		GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "Blocked");
		InDamageInfo.IsAbsorbed = true;
		this->BlockHitDelegate.Broadcast(HitInstigator, InDamageInfo);
	}

	if (InDamageInfo.IsAbsorbed && this->CrossStateData.IsParry == true) return;

	if (this->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectReceiveHit(this->CompContext->CharacterActor, HitInstigator, InDamageInfo);
		if (ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CompContext->CharacterActor, EAttributeCode::ATT_Health) <= 0) {
			if (this->CompContext->Controller) {
				this->CompContext->Controller->UnPossess();
			}
			this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Death"), EActionList::ActionNone, EInputEvent::IE_Released);
			return;
		}
		
		ICharacterSystemAvailable::Execute_InitEffectFortitudeDamage(this->CompContext->CharacterActor, HitInstigator, InDamageInfo);
		float CurrentFortitude = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CompContext->CharacterActor, EAttributeCode::ATT_Fortitude);
		float MaxFortitude = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(this->CompContext->CharacterActor, EAttributeCode::ATT_Fortitude);
		GLog->Log("xxxss");
		if ((CurrentFortitude > 0 || !this->IsStaggeredOnEmptyFortitude ) && !InDamageInfo.IsAbsorbed && InDamageInfo.HitType != EDamageHitType::PARRY) {
			GLog->Log("whatevffa");
			if (InDamageInfo.ImpactType >= this->MinimumImpactForKnockback && (!this->CrossStateData.IsInHyperArmor || (this->CrossStateData.IsInHyperArmor && CurrentFortitude < MaxFortitude / 2))) {
				GLog->Log("whatevffa2asd");
				this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked"), EActionList::ActionNone, EInputEvent::IE_Released);
			}
			else {
				//Sponge: jitter
			}
		}
		else if (CurrentFortitude <= 0 && this->IsStaggeredOnEmptyFortitude) {
			this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Staggered"), EActionList::ActionNone, EInputEvent::IE_Released);
		}

	}
	else {
		this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked"), EActionList::ActionNone, EInputEvent::IE_Released);
	}

}

void UCharacterStatesSystem::OnMovementModeChanged(ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	auto CharMove = Cast<UCharacterMovementComponent>(this->CompContext->MovementComp);
	if (CharMove->MovementMode == EMovementMode::MOVE_Walking || CharMove->MovementMode == EMovementMode::MOVE_NavWalking) {
		this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
	}
	else if (CharMove->MovementMode == EMovementMode::MOVE_Falling) {
		this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.InAir"), EActionList::ActionNone, IE_Released);
	}
}

//sponge: dunno if it's a good way to put it here, also might need to clear delegate
void UCharacterStatesSystem::LockAnimation(EDamageImpactType InDamageImpactTime, FHitStopFinishDelegate OnHitFinish)
{
	if (this->CrossStateData.IsHitStopped) return;
	this->CrossStateData.IsHitStopped = true;
	if (this->CompContext->CharacterAnim && this->CompContext->CharacterAnim->GetCurrentActiveMontage()) {
		this->CompContext->CharacterAnim->Montage_Pause(this->CompContext->CharacterAnim->GetCurrentActiveMontage());
	}
	TMap<EDamageImpactType, float> LockTimeMap = {
		{ EDamageImpactType::DI_LOW, 0.07 },
		{ EDamageImpactType::DI_MEDIUM, 0.1 },
		{ EDamageImpactType::DI_HIGH, 0.2 },
		{ EDamageImpactType::DI_EXPLOSIVE, 0.25 },
	};

	float LockTime = LockTimeMap[InDamageImpactTime];
	this->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->AnimLockHandle, FTimerDelegate::CreateLambda([&, OnHitFinish] {
		this->CrossStateData.IsHitStopped = false;
		this->BlockedTags.RemoveTag(FGameplayTag::RequestGameplayTag("ActionStates.Knocked.Stand"));
		OnHitFinish.ExecuteIfBound();
		if (this->CompContext->CharacterAnim && this->CompContext->CharacterAnim->GetCurrentActiveMontage()) {
			this->CompContext->CharacterAnim->Montage_Resume(this->CompContext->CharacterAnim->GetCurrentActiveMontage());
		}
	}), LockTime, false);
}
