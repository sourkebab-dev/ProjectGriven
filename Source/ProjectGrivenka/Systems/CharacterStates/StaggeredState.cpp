// Fill out your copyright notice in the Description page of Project Settings.


#include "StaggeredState.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void UStaggeredState::Init_Implementation(FCharacterContext InContext, UCharacterStatesSystem* InStatesComp)
{
	Super::Init_Implementation(InContext, InStatesComp);
	this->CharacterContext.EventBus->StaggerDelegate.AddDynamic(this, &UStaggeredState::OnReceiveStagger);
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
	this->CharacterContext.CharacterAnim->Montage_Play(this->StaggerMontage);
	FOnMontageEnded EndAttackDelegate;
	EndAttackDelegate.BindUObject(this, &UStaggeredState::OnStaggerEnd);
	this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(EndAttackDelegate, this->StaggerMontage);
	this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndAttackDelegate, this->StaggerMontage);

}

void UStaggeredState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
}

void UStaggeredState::AxisHandler_Implementation(EActionList Action, float AxisValue)
{
}

void UStaggeredState::OnStateExit_Implementation()
{

	if (!this->CharacterContext.CharacterAnim) return;
	FAnimMontageInstance* AnimMontageInstance = this->CharacterContext.CharacterAnim->GetActiveInstanceForMontage(this->StaggerMontage);
	if (AnimMontageInstance) {
		AnimMontageInstance->OnMontageEnded.Unbind();
		AnimMontageInstance->OnMontageBlendingOutStarted.Unbind();
	}
	this->CharacterContext.CharacterAnim->Montage_Stop(0.25);

}

void UStaggeredState::OnStaggerEnd(UAnimMontage* Montage, bool bInterrupted)
{
	this->StaggerInstigator = nullptr;
	this->StaggerInfo = FDamageInfo();
	if (!bInterrupted) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, EInputEvent::IE_Released);
	}
}
