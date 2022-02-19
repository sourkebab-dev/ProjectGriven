// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextHelperLib.h"
#include "Components/CapsuleComponent.h"
#include "ProjectGrivenka/Systems/ContextSystem.h"


void UContextHelperLib::SetDamageCollider(class UContextSystem* Ctx, bool IsActive)
{
	if (!Ctx) return;
	TArray<USceneComponent*> DamageColliders;
	Ctx->SkeletalMeshComp->GetChildrenComponents(false, DamageColliders);
	for (int i = 0; i < DamageColliders.Num(); i++) {
		auto Collider = Cast<UCapsuleComponent>(DamageColliders[i]);
		if (!Collider) continue;
		Collider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, IsActive ? ECollisionResponse::ECR_Overlap : ECollisionResponse::ECR_Ignore);
	}
}
