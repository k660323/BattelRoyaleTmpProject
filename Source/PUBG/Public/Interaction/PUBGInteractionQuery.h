// Kakanu Portfolio Project

#pragma once

#include "PUBGInteractionQuery.generated.h"

USTRUCT(BlueprintType)
struct FPUBGInteractionQuery
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> RequestingAvatar;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AController> RequestingController;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UObject> OptionalObjectData;
};