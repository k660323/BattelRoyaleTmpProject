// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_StateTransformStart.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_StateTransformStart : public UPUBGGameplayAbility
{
	GENERATED_BODY()
public:
	UPUBGGA_StateTransformStart(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
