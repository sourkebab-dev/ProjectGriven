// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ContextHelperLib.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIVENKA_API UContextHelperLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void SetDamageCollider(class UContextSystem* Ctx, bool IsActive);

};
