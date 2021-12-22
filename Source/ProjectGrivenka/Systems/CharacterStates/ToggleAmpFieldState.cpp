// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleAmpFieldState.h"
#include "ProjectGrivenka/GrivenkaSingletonLibrary.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void UToggleAmpFieldState::RaiseAmpField()
{
	this->StatesComp->CrossStateData.IsAmpActivated = true;
	this->CharacterContext.EventBus->EffectRemoveDelegate.Broadcast(FGameplayTag::RequestGameplayTag("CharacterSystem.Effects.Defaults.Regen.Amp"));
	this->EndAmpAction();
}

void UToggleAmpFieldState::LowerAmpField()
{
	this->StatesComp->CrossStateData.IsAmpActivated = false;
	FRPGEffectInitDelegate* EffectInitDelegate = this->CharacterContext.EventBus->EffectApplyObservers.Find(EEffectDelegates::EDL_START_AMP_REGEN);
	if (EffectInitDelegate) EffectInitDelegate->Broadcast(1, this->CharacterContext.CharacterActor, this->CharacterContext.CharacterActor);
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
	//this->CharacterContext.CharacterAnim->Montage_SetEndDelegate(EndMontageDelegate, this->CharacterInstance->CommonAnimationData.ToggleAmpField);
	//this->CharacterContext.CharacterAnim->Montage_SetBlendingOutDelegate(EndMontageDelegate, this->CharacterInstance->CommonAnimationData.ToggleAmpField);

}

void UToggleAmpFieldState::OnStateExit_Implementation()
{
	UGrivenkaDataSingleton* CommonData = UGrivenkaSingletonLibrary::GetGrivenkaData();
	//this->CharacterInstance->SetRotationRate(CommonData->CommonRotationRate.NormalRotationRate);
	this->CharacterContext.CharacterAnim->StopAllMontages(0.25);
}
