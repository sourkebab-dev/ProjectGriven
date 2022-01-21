// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleAmpFieldState.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

void UToggleAmpFieldState::RaiseAmpField()
{
	this->StatesComp->CrossStateData.IsAmpActivated = true;
	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_RemoveEffectByTag(this->StatesComp->CompContext->CharacterActor, FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Defaults.Regen.Amp"));
	}
	this->EndAmpAction();
}

void UToggleAmpFieldState::LowerAmpField()
{
	this->StatesComp->CrossStateData.IsAmpActivated = false;
	if (this->StatesComp->CompContext->CharacterActor->Implements<UCharacterSystemAvailable>()) {
		ICharacterSystemAvailable::Execute_InitEffectByRegenName(this->StatesComp->CompContext->CharacterActor, this->StatesComp->CompContext->CharacterActor, "Util_AmpRegen");
	}
	this->EndAmpAction();
}

void UToggleAmpFieldState::EndAmpAction()
{
	this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
}

void UToggleAmpFieldState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.OffRotationRate);
	//this->CharacterInstance->CharacterContext.CharacterAnim->Montage_Play(this->CharacterInstance->CommonAnimationData.ToggleAmpField);
	FOnMontageEnded EndMontageDelegate;
	if (this->StatesComp->CrossStateData.IsAmpActivated) {
		EndMontageDelegate.BindUFunction(this, "LowerAmpField");
	}
	else {
		EndMontageDelegate.BindUFunction(this, "RaiseAmpField");
	}
	//this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndMontageDelegate, this->CharacterInstance->CommonAnimationData.ToggleAmpField);
	//this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndMontageDelegate, this->CharacterInstance->CommonAnimationData.ToggleAmpField);

}

void UToggleAmpFieldState::OnStateExit_Implementation()
{
	UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
	this->StatesComp->CompContext->CharacterAnim->StopAllMontages(0.25);
}
