// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackSignaller.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectGrivenka/AI/BaseAIController.h"

void UAttackSignaller::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!Mesh) return;
	AActor* ActorInstigator = Mesh->GetOwner();
	if (!ActorInstigator) return;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(ActorInstigator);
	TArray<FHitResult> OutResults;


	//Sponge: Tracetype might be bad performance
	if (UKismetSystemLibrary::SphereTraceMulti(ActorInstigator->GetWorld(), ActorInstigator->GetActorLocation(), ActorInstigator->GetActorLocation(), 300.0f,
		UEngineTypes::ConvertToTraceType(ECC_Pawn), false, IgnoredActors, EDrawDebugTrace::None, OutResults, true)) {
		for (int i = 0; i < OutResults.Num(); i++) {
			AActor* HitActor = OutResults[i].Actor.Get();
			APawn* HitPawn = Cast<APawn>(HitActor);
			if (!HitActor || !HitPawn || !HitPawn->GetController()) continue;
			FVector InstigatorForwardVec = ActorInstigator->GetActorForwardVector();
			FVector InstigatorToTargetVec = HitActor->GetActorLocation() - ActorInstigator->GetActorLocation();
			FVector TargetForwardVector = HitActor->GetActorForwardVector();

			if (FVector::DotProduct(InstigatorForwardVec, InstigatorToTargetVec) > 0 && FVector::DotProduct(TargetForwardVector, InstigatorForwardVec) < 0) {
				ABaseAIController* AIController = Cast<ABaseAIController>(HitPawn->GetController());
				if (!AIController) return;
				AIController->SendAIEvent(EAIEvent::SIGNALLED_ATTACK);
			}
		}
	}
}
