// Kakanu Portfolio Project


#include "Item/FeatureConditions/EquipmentCondition/PUBGFeatureEquipmentCondition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGFeatureEquipmentCondition)

UPUBGFeatureEquipmentCondition::UPUBGFeatureEquipmentCondition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(const TArray<TSubclassOf<UPUBGFeatureEquipmentCondition>>& EquipmentFeatureConditionClasses, UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem)
{
	for (const TSubclassOf<UPUBGFeatureEquipmentCondition>& EquipmentFeatureConditionClass : EquipmentFeatureConditionClasses)
	{
		if (EquipmentFeatureConditionClass == nullptr)
		{
			continue;
		}

		UPUBGFeatureEquipmentCondition* FeatureCondition = EquipmentFeatureConditionClass->GetDefaultObject<UPUBGFeatureEquipmentCondition>();

		if (FeatureCondition == nullptr)
		{
			continue;
		}

		if (FeatureCondition->CanUseFeature(Owner, UnequipItem, EquipItem) == false)
		{
			return false;
		}
	}

	return true;
}

bool UPUBGFeatureEquipmentCondition::CanUseFeature(UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem) const
{
	return false;
}
