// Fill out your copyright notice in the Description page of Project Settings.


#include "StaggeredState.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void UStaggeredState::Init_Implementation(UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InStatesComp);
	this->StatesComp->CompContext->EventBus->StaggerDelegate.AddDynamic(this, &UStaggeredState::OnReceiveStagger);
}


void UStaggeredState::OnReceiveStagger(AActor* InHitInstigator, FDamageInfo InDamageInfo)
{
	this->StaggerInfo = InDamageInfo;
	this->StaggerInstigator = InHitInstigator;
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Staggered"), EActionList::ActionNone, IE_Released);
}

void UStaggeredState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->StaggerMontage);
	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UStaggeredState::OnStaggerEnd);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->StaggerMontage);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->StaggerMontage);

	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->HitStopTimer, FTimerDelegate::CreateLambda([&] {
		if (this->StaggerInstigator) {
			auto InstigatorCtx = IContextAvailable::Execute_GetContext(this->StaggerInstigator);
			InstigatorCtx->EventBus->HitStopDelegate.Execute(EDamageImpactType::DI_HIGH, FHitStopFinishDelegate::CreateLambda([] {}));
		}

		this->StatesComp->LockAnimation(EDamageImpactType::DI_HIGH, FHitStopFinishDelegate::CreateLambda([&] { })); 
	}), 0.005, false);
}

void UStaggeredState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
}

void UStaggeredState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
}

void UStaggeredState::OnStateExit_Implementation()
{
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->HitStopTimer);

	if (!this->StatesComp->CompContext->CharacterAnim) return;
	FAnimMontageInstance* AnimMontageInstance = this->StatesComp->CompContext->CharacterAnim->GetActiveInstanceForMontage(this->StaggerMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->StatesComp->CompContext->CharacterAnim->Montage_Stop(0.25);

}

void UStaggeredState::OnStaggerEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted) {
		this->StaggerInstigator = nullptr;
		this->StaggerInfo = FDamageInfo();
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, EInputEvent::IE_Released);
	}
}
