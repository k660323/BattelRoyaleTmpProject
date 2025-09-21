// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_UnActiveEquipment.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_UnActiveEquipment : public UPUBGGameplayAbility
{
	GENERATED_BODY()

public:
	UPUBGGA_UnActiveEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnEndMontage();
};
