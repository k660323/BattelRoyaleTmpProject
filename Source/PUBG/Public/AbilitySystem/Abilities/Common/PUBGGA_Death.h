// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_Death.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Death : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
