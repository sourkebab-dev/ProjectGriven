// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockedState.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"

void UKnockedState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{
	//Sponge : need to replace && this->EnterAction == EActionList::ActionLowKnocked
	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && this->StatesComp->CrossStateData.IsInterruptable) {
		this->CharacterContext.CharacterAnim->Montage_Stop(0.25);
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Dodge"), EActionList::ActionDodge, IE_Pressed);
	}
}

void UKnockedState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	this->StatesComp->CrossStateData.IsInterruptable = false;
	this->CharacterContext.CharacterActor->GetWorldTimerManager().SetTimer(this->StunTimer, this, &UKnockedState::OnStunned, this->CharacterContext.CharacterActor->GetWorld()->GetDeltaSeconds(), true, -1.0f);
}

void UKnockedState::OnStateExit_Implementation()
{
	this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->StunTimer);
	this->StatesComp->CrossStateData.IsInterruptable = true;
	this->PooledStunTime = 0.0;
}


void UKnockedState::OnStunned() {
	this->PooledStunTime += this->CharacterContext.CharacterActor->GetWorld()->GetDeltaSeconds();
	if (this->PooledStunTime / this->StunTime >= 0.5) this->StatesComp->CrossStateData.IsInterruptable = true;
	if (this->PooledStunTime >= this->StunTime) {
		this->CharacterContext.CharacterActor->GetWorldTimerManager().ClearTimer(this->StunTimer);
		this->OnFinishStunned();
	}
}

void UKnockedState::OnFinishStunned() {
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
}