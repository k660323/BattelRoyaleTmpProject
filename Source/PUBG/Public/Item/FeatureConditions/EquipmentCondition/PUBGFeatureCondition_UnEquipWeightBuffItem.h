// Kakanu Portfolio Project

#pragma once

#include "Item/FeatureConditions/EquipmentCondition/PUBGFeatureEquipmentCondition.h"
#include "PUBGFeatureCondition_UnEquipWeightBuffItem.generated.h"

class UPUBGItemInstance;

UCLASS(BlueprintType, Blueprintable)
class UPUBGFeatureCondition_UnEquipWeightBuffItem : public UPUBGFeatureEquipmentCondition
{
	GENERATED_BODY()

public:
	UPUBGFeatureCondition_UnEquipWeightBuffItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual bool CanUseFeature(UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem) const override;
};

