// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_EquipBackpack.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_EquipBackpack : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_EquipBackpack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void ReplaceWeaponSocket();
};
