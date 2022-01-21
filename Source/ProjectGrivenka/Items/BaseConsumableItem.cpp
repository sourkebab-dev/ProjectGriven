// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseConsumableItem.h"
#include "ProjectGrivenka/Interfaces/ContextAvailable.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"
#include "ProjectGrivenka/ContextUtilities/EventBus.h"

void ABaseConsumableItem::OnUse()
{
	Super::OnUse();
	if (!this->GetOwner() || !this->GetOwner()->Implements<UContextAvailable>()) { GLog->Log("owner invalid"); return; }
	auto CharCtx = IContextAvailable::Execute_GetContext(this->GetOwner());
	AActor* EffectReceiverActor = Cast<AActor>(this->GetOwner());

	if (!CharCtx->EventBus) { GLog->Log("EventBusInvalid OnUse Item"); return; }
	if (!this->GetOwner()->Implements<UContextAvailable>() || !EffectReceiverActor) { GLog->Log("owner invalid"); return; }

	for (int i = 0; i < this->ItemInfo.Effects.Num(); i++) {
		CharCtx->EventBus->ItemUsageDelegate.Broadcast(EffectReceiverActor, this->ItemInfo.Effects[i]);
	}
}
