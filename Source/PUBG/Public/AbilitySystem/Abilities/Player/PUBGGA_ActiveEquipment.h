// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_ActiveEquipment.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_ActiveEquipment : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_ActiveEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void OnActiveEquipmentEndMontage();

private:
	UPROPERTY()
	bool bSwap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EActiveWeaponEquipmentType ActiveEquipmentState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> SwapAbilityClass;

	UPROPERTY()
	FGameplayTagContainer TagsToCancelOnActivateAbility;
};
