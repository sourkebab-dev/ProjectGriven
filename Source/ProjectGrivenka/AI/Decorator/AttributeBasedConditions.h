// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemDefinitions.h"
#include "AttributeBasedConditions.generated.h"


UENUM(BlueprintType)
enum EComparison {
    MORE_THAN       UMETA(DisplayName = "More Than"),
    LESS_THAN       UMETA(DisplayName = "Less Than"),
    MORE_EQ_THAN       UMETA(DisplayName = "Equal/More Than"),
    LESS_EQ_THAN UMETA(DisplayName = "Equal/Less Than"),
    EQUALS UMETA(DisplayName ="Equals"),
};

UCLASS()
class PROJECTGRIVENKA_API UAttributeBasedConditions : public UBTDecorator
{
    GENERATED_BODY()
   
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EAttributeCode> Attribute;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EComparison> Comparison;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isPercentage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CompareValue;

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
