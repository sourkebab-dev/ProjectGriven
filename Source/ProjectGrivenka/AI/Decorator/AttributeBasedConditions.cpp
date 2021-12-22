// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeBasedConditions.h"
#include "ProjectGrivenka/Systems/CharacterSystem/CharacterSystemAvailable.h"
#include "ProjectGrivenka/AI/BaseAIController.h"

bool UAttributeBasedConditions::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{

	APawn* ThisPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ThisPawn) return false;

	if (!ThisPawn->Implements<UCharacterSystemAvailable>()) return false;
	float CurrentAttribute = ICharacterSystemAvailable::Execute_GetAttributeCurrentValue(ThisPawn, this->Attribute);
	float MaxAttribute = ICharacterSystemAvailable::Execute_GetAttributeMaxValue(ThisPawn, this->Attribute);

	switch (this->Comparison)
	{
		case EComparison::EQUALS:
			return this->isPercentage ? CurrentAttribute / MaxAttribute * 100 == this->CompareValue : CurrentAttribute == this->CompareValue;
		case EComparison::LESS_EQ_THAN:
			return this->isPercentage ? CurrentAttribute / MaxAttribute * 100 <= this->CompareValue : CurrentAttribute <= this->CompareValue;
		case EComparison::LESS_THAN:
			return this->isPercentage ? CurrentAttribute / MaxAttribute * 100 < this->CompareValue : CurrentAttribute < this->CompareValue;
		case EComparison::MORE_THAN:
			return this->isPercentage ? CurrentAttribute / MaxAttribute * 100 > this->CompareValue : CurrentAttribute > this->CompareValue;
		case EComparison::MORE_EQ_THAN:
			return this->isPercentage ? CurrentAttribute / MaxAttribute * 100 >= this->CompareValue : CurrentAttribute >= this->CompareValue;
		default:
			break;
	}
	return false;
}
