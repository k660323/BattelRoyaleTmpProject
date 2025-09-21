// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Item/PUBGItemTemplate.h"
#include "PUBGItemFragment_Payload.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PUBG_API UPUBGItemFragment_Payload : public UPUBGItemFragment
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Payload(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	EPayloadType PayloadType;
};
