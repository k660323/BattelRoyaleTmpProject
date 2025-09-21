// Kakanu Portfolio Project


#include "Item/FeatureConditions/ConsumeCondition/PUBGFeatureConsumeCondition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGFeatureConsumeCondition)

UPUBGFeatureConsumeCondition::UPUBGFeatureConsumeCondition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UPUBGFeatureConsumeCondition::ExecuteEquipmentFeatureConditions(const TArray<TSubclassOf<UPUBGFeatureConsumeCondition>>& ConsumeFeatureConditionClasses, UObject* Owner)
{
	for (const TSubclassOf<UPUBGFeatureConsumeCondition>& ConsumeFeatureConditionClass : ConsumeFeatureConditionClasses)
	{
		if (ConsumeFeatureConditionClass == nullptr)
		{
			continue;
		}

		UPUBGFeatureConsumeCondition* FeatureCondition = ConsumeFeatureConditionClass->GetDefaultObject<UPUBGFeatureConsumeCondition>();

		if (FeatureCondition == nullptr)
		{
			continue;
		}

		if (FeatureCondition->CanUseFeature(Owner) == false)
		{
			return false;
		}
	}

	return true;
}

bool UPUBGFeatureConsumeCondition::CanUseFeature(UObject* Owner) const
{
	return false;
}
