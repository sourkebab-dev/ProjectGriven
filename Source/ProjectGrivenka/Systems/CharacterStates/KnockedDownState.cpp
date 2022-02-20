// Fill out your copyright notice in the Description page of Project Settings.


#include "KnockedDownState.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

void UKnockedDownState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType) 
{
	if (Action == EActionList::ActionDodge && EventType == IE_Pressed && !this->IsRecovering) {
		this->IsRecovering = true;
		float RecoverTime = 0.0;
		if (FVector::DotProduct(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir, this->StatesComp->CompContext->CharacterActor->GetActorRightVector()) > 0.2) {
			this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("RecoverRight", this->StatesComp->CrossStateData.KnockDownMontage);
			RecoverTime = this->StatesComp->CrossStateData.KnockDownMontage->GetSectionLength(this->StatesComp->CrossStateData.KnockDownMontage->GetSectionIndex("RecoverRight"));
		}
		else if (FVector::DotProduct(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir, this->StatesComp->CompContext->CharacterActor->GetActorRightVector() * -1) > 0.2) {
			this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("RecoverLeft", this->StatesComp->CrossStateData.KnockDownMontage);
			RecoverTime = this->StatesComp->CrossStateData.KnockDownMontage->GetSectionLength(this->StatesComp->CrossStateData.KnockDownMontage->GetSectionIndex("RecoverLeft"));
		}
		else if (FVector::DotProduct(this->StatesComp->CompContext->MovementModule.WorldSpaceTargetDir, this->StatesComp->CompContext->CharacterActor->GetActorUpVector() * -1) > 0.2) {
			this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("RecoverBack", this->StatesComp->CrossStateData.KnockDownMontage);
			RecoverTime = this->StatesComp->CrossStateData.KnockDownMontage->GetSectionLength(this->StatesComp->CrossStateData.KnockDownMontage->GetSectionIndex("RecoverBack"));
		}
		else {
			this->StatesComp->CrossStateData.KnockDownMontage->GetSectionLength(this->StatesComp->CrossStateData.KnockDownMontage->GetSectionIndex("RecoverFront"));
			this->StatesComp->CompContext->CharacterAnim->Montage_JumpToSection("RecoverFront", this->StatesComp->CrossStateData.KnockDownMontage);
			RecoverTime = this->StatesComp->CrossStateData.KnockDownMontage->GetSectionLength(this->StatesComp->CrossStateData.KnockDownMontage->GetSectionIndex("RecoverFront"));
		}

		this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().SetTimer(this->RecoverTimer, FTimerDelegate::CreateLambda([&] {
			this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		}), RecoverTime, false);
	}
}

void UKnockedDownState::OnStateEnter_Implementation()
{
	if (!this->StatesComp->CrossStateData.KnockDownMontage) {
		this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
		return;
	}
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(this->StatesComp->CrossStateData.KnockDownMontage);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::OFF_ROTATION);
}

void UKnockedDownState::OnStateExit_Implementation()
{
	this->StatesComp->CrossStateData.KnockDownMontage = nullptr;
	this->StatesComp->CompContext->CharacterActor->GetWorldTimerManager().ClearTimer(this->RecoverTimer);
	this->StatesComp->CompContext->CharacterAnim->Montage_Stop(0.25);
	this->StatesComp->CompContext->EventBus->AnimDelegate.Broadcast(EAnimEvt::FULL_ROTATION);
}
