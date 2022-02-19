// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatesSystem.h"
#include "BaseState.h"
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

	if (this->CurrentState->ActionTag.HasTag(FGameplayTag::RequestGameplayTag("ActionStates.Block")) 
		&& UVectorMathLib::CheckBlockDirection(HitInstigator->GetActorLocation(), this->CompContext->CharacterActor->GetActorLocation(), this->CompContext->CharacterActor->GetActorForwardVector())) {
		GEngine->AddOnScreenDebugMessage(12, 2, FColor::Yellow, "Blocked");
		InDamageInfo.IsAbsorbed = true;
		this->BlockHitDelegate.Broadcast(HitInstigator, InDamageInfo);
	}

	if (this->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectReceiveHit(this->CompContext->CharacterActor, HitInstigator, InDamageInfo);

		if (ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CompContext->CharacterActor, EAttributeCode::ATT_Health) <= 0) {
			if (this->CompContext->Controller) {
				this->CompContext->Controller->UnPossess();
			}
			this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Death"), EActionList::ActionNone, EInputEvent::IE_Released);
			return;
		}

		if (!this->CrossStateData.IsParry) {
			ICharacterSystemAvailable::Execute_InitEffectFortitudeDamage(this->CompContext->CharacterActor, HitInstigator, InDamageInfo);
			float CurrentFortitude = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(this->CompContext->CharacterActor, EAttributeCode::ATT_Fortitude);
			float MaxFortitude = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(this->CompContext->CharacterActor, EAttributeCode::ATT_Fortitude);

			//Note: if actor is launched && launchavailable -> skip stagger & knock
			if (InDamageInfo.ImpactType >= this->LaunchByWhichImpact 
				&& this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.KnockedLaunch"), EActionList::ActionNone, EInputEvent::IE_Released)) {
				return;
			}


			if (CurrentFortitude <= 0 && ( this->IsStaggeredOnEmptyFortitude || InDamageInfo.HitType != EDamageHitType::DEFAULT)) {
				this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Staggered"), EActionList::ActionNone, EInputEvent::IE_Released);
			}
			else if (CurrentFortitude / MaxFortitude < 0.6 && InDamageInfo.HitType != EDamageHitType::PARRY) {
				this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked"), EActionList::ActionNone, EInputEvent::IE_Released);
			}
			else {
				//Sponge: need to add jitter effect
			}
		}
	}
	else {
		this->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Knocked"), EActionList::ActionNone, EInputEvent::IE_Released);
	}

}

//sponge: dunno if it's a good way to put it here, also might need to clear delegate
void UCharacterStatesSystem::LockAnimation(EDamageImpactType InDamageImpactTime, FHitStopFinishDelegate OnHitFinish)
{
	if (this->CrossStateData.IsHitStopped) return;
	this->CrossStateData.IsHitStopped = true;
	this->CompContext->CharacterAnim->Montage_Pause(this->CompContext->CharacterAnim->GetCurrentActiveMontage());
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
		this->CompContext->CharacterAnim->Montage_Resume(this->CompContext->CharacterAnim->GetCurrentActiveMontage());
	}), LockTime, false);
}
