// Kakanu Portfolio Project

#pragma once

#include "UObject/NoExportTypes.h"
#include "PUBGFeatureEquipmentCondition.generated.h"

class UPUBGItemInstance;

/**
 * 
 */
 // DefaultToInstanced : Ŭ������ ��������� ������ �ڵ����� �ν��Ͻ� ����
UCLASS(BlueprintType, Abstract)
class PUBG_API UPUBGFeatureEquipmentCondition : public UObject
{
	GENERATED_BODY()
	
public:
	UPUBGFeatureEquipmentCondition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool ExecuteEquipmentFeatureConditions(const TArray<TSubclassOf<UPUBGFeatureEquipmentCondition>>& EquipmentFeatureConditionClasses, UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem);

public:
	// ������ ��������
	virtual bool CanUseFeature(UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem) const;
};
