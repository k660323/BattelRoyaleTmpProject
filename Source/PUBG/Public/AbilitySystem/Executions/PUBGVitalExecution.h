// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PUBGVitalExecution.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGVitalExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UPUBGVitalExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
