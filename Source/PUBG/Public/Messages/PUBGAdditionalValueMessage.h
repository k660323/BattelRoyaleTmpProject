#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PUBGAdditionalValueMessage.generated.h"


USTRUCT(BlueprintType)
struct FPUBGAdditionalValueMessage
{
	GENERATED_BODY()
public:
	FPUBGAdditionalValueMessage() :
		EventTag(), ChangedAdditionalValue(0.0f) {};

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTag EventTag;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	float ChangedAdditionalValue;
};