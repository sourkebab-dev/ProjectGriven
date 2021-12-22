// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ProjectGrivenka/PersistedData/CharacterData.h"
#include "GiveItem.generated.h"

/**
 * 
 */

//Might need to move this shit somewhere else
USTRUCT(BlueprintType)
struct FItemGiven
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UItemPrefab* GivenItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemCount;
};


UCLASS()
class PROJECTGRIVENKA_API UGiveItem : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	//Sponge: need to change item  to new system
	UPROPERTY(EditAnywhere, Category = ItemData)
	TArray<FItemGiven> ItemData;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UGiveItem();
};
