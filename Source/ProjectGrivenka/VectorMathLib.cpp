// Fill out your copyright notice in the Description page of Project Settings.


#include "VectorMathLib.h"
#include "Kismet/KismetMathLibrary.h"

float UVectorMathLib::DegreesBetweenVectors(FVector A, FVector B)
{
	float DotResult = FVector::DotProduct(A, B);
	float Sign = FMath::Sign(FVector::CrossProduct(A, B).Z);
	return Sign * UKismetMathLibrary::DegAcos(DotResult);
}

bool UVectorMathLib::CheckBlockDirection(FVector InstigatorLocation, FVector ReceiverLocation, FVector ReceiverForward)
{
	FVector ToAttackerVector = InstigatorLocation - ReceiverLocation;
	ToAttackerVector.Normalize();
	ReceiverForward.Normalize();
	return FVector::DotProduct(ToAttackerVector, ReceiverForward) >= 0;
}

void UVectorMathLib::RotateActorToTargetVector(AActor* ToRotateActor, FVector TargetVector, float RotationRate, float DeltaSeconds)
{
	FRotator CurrentRotation = ToRotateActor->GetActorRotation();
	FVector CurrentLocation = ToRotateActor->GetActorLocation();
	FRotator RotationTarget = FRotator::ZeroRotator;
	RotationTarget.Yaw = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, TargetVector).Yaw;
	FRotator InterpRotation = FMath::RInterpTo(CurrentRotation, RotationTarget, DeltaSeconds, RotationRate);
	ToRotateActor->SetActorRotation(InterpRotation);
}
