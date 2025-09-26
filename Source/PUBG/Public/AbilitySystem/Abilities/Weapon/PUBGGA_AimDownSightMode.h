// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/Player/PUBGGA_Equipment.h"
#include "PUBGGA_AimDownSightMode.generated.h"

class UCameraMode;

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_AimDownSightMode : public UPUBGGA_Equipment
{
	GENERATED_BODY()
	
public:
	UPUBGGA_AimDownSightMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraMode> InADSCameraMode;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraMode> OutADSCameraMode;
};
