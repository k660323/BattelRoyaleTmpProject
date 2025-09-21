// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "PUBGPlayerGA_Walk.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGA_Walk : public UPUBGPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGPlayerGA_Walk(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnReInputRelease(float Time);
};
