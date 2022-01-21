// Fill out your copyright notice in the Description page of Project Settings.


#include "OneOffUseItemState.h"
#include "RootUseItemState.h"
#include "ProjectGrivenka/Items/BaseItem.h"
#include "ProjectGrivenka/Systems/InventorySystem/CharacterInventoryAvailable.h"
#include "ProjectGrivenka/Systems/CharacterStates/CharacterStatesSystem.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"

void UOneOffUseItemState::ActionHandler_Implementation(EActionList Action, EInputEvent EventType)
{

}

void UOneOffUseItemState::OnStateEnter_Implementation(FGameplayTagContainer InPrevActionTag, EActionList NewEnterAction, EInputEvent NewEnterEvent)
{
	Super::OnStateEnter_Implementation(InPrevActionTag, NewEnterAction, NewEnterEvent);
	ABaseItem* UsedItem = ICharacterInventoryAvailable::Execute_GetCommitedItem(this->StatesComp->CompContext->CharacterActor);
	this->StatesComp->CompContext->CharacterAnim->Montage_Play(UsedItem->ItemInfo.UsageAnimation);
	FOnMontageEnded EndUseItemDelegate;
	EndUseItemDelegate.BindUFunction(this, "OnUseItemAnimFinished");
	this->StatesComp->CompContext->CharacterAnim->Montage_SetEndDelegate(EndUseItemDelegate, UsedItem->ItemInfo.UsageAnimation);
	this->StatesComp->CompContext->CharacterAnim->Montage_SetBlendingOutDelegate(EndUseItemDelegate, UsedItem->ItemInfo.UsageAnimation);
}

void UOneOffUseItemState::OnStateExit_Implementation()
{
}

void UOneOffUseItemState::OnUseItemAnimFinished()
{
	//note: might be buggy
	UBaseState* RootState = Cast<UBaseState>(this->GetOuter());
	if(RootState) this->StatesComp->ChangeState(FGameplayTag::RequestGameplayTag("ActionStates.Default"), EActionList::ActionNone, IE_Released);
}
