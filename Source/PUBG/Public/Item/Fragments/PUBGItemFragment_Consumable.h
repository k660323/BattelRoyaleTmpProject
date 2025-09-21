// Kakanu Portfolio Project

#pragma once

#include "Item/PUBGItemTemplate.h"
#include "PUBGDefine.h"
#include "PUBGItemFragment_Consumable.generated.h"

class UPUBGItemInstance;
class UGameplayEffect;
class UPUBGFeatureConsumeCondition;

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Consumable : public UPUBGItemFragment
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Consumable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EConsumptionType ConsumptionType = EConsumptionType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ConsumptionEffectClass;

	UPROPERTY(EditDefaultsOnly)
	float UseTime = 0.0f;

	// Instanced : 개별 인스턴스 허용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UPUBGFeatureConsumeCondition>> FeatureConditions;
};
