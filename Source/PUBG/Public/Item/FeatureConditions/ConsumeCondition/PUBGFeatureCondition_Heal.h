#pragma once

#include "GameplayEffect.h"
#include "Item/FeatureConditions/ConsumeCondition/PUBGFeatureConsumeCondition.h"
#include "PUBGFeatureCondition_Heal.generated.h"

class UAttributeSet;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PUBG_API UPUBGFeatureCondition_Heal : public UPUBGFeatureConsumeCondition
{
	GENERATED_BODY()

public:
	UPUBGFeatureCondition_Heal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual bool CanUseFeature(UObject* Owner) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute TargetAttribute;

	UPROPERTY(EditDefaultsOnly)
	float Limit;
};
