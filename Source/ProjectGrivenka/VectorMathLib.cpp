// Fill out your copyright notice in the Description page of Project Settings.


#include "VectorMathLib.h"
#include "Kismet/KismetMathLibrary.h"

float UVectorMathLib::DegreesBetweenVectors(FVector A, FVector B)
{
	float DotResult = FVector::DotProduct(A, B);
	float Sign = FMath::Sign(FVector::CrossProduct(A, B).Z);
	return Sign * UKismetMathLibrary::DegAcos(DotResult);
}
