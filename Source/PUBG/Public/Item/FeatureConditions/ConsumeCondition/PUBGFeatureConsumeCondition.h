// Kakanu Portfolio Project

#pragma once

#include "UObject/NoExportTypes.h"
#include "PUBGFeatureConsumeCondition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Abstract)
class PUBG_API UPUBGFeatureConsumeCondition : public UObject
{
	GENERATED_BODY()
	
public:
	UPUBGFeatureConsumeCondition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool ExecuteEquipmentFeatureConditions(const TArray<TSubclassOf<UPUBGFeatureConsumeCondition>>& ConsumeFeatureConditionClasses, UObject* Owner);

public:
	// ������ ��������
	virtual bool CanUseFeature(UObject* Owner) const;
};
