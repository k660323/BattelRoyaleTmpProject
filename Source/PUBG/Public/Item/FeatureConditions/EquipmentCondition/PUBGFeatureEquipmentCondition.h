// Kakanu Portfolio Project

#pragma once

#include "UObject/NoExportTypes.h"
#include "PUBGFeatureEquipmentCondition.generated.h"

class UPUBGItemInstance;

/**
 * 
 */
 // DefaultToInstanced : 클래스를 멤버변수로 가질시 자동으로 인스턴스 생성
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
	// 아이템 장착관련
	virtual bool CanUseFeature(UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem) const;
};
