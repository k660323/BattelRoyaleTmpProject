// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_StateTransformTimer.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_StateTransformTimer : public UPUBGGameplayAbility
{
	GENERATED_BODY()
public:
	UPUBGGA_StateTransformTimer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	FTimerHandle TimerHandle;
};
