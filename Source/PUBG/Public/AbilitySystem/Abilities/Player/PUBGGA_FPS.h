// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_FPS.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_FPS : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_FPS(const FObjectInitializer& ObjectInitalizer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnPressed(float TimeWaited);
};
