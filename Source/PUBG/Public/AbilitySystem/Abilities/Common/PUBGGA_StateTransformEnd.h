// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_StateTransformEnd.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_StateTransformEnd : public UPUBGGameplayAbility
{
	GENERATED_BODY()
public:
	UPUBGGA_StateTransformEnd(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
