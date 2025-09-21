// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_SwapActiveEquipment.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_SwapActiveEquipment : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_SwapActiveEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void UnActiveEquipment();
	void ActiveEquipment();

	EActiveWeaponEquipmentType GetActiveEquipmentState() const;

private:
	UFUNCTION()
	void OnEndUnActiveEquipmentMontage();

	UFUNCTION()
	void OnUnActiveEquipmentNetSync();

	UFUNCTION()
	void OnEndActiveEquipmentMontage();

private:
	UPROPERTY()
	EActiveWeaponEquipmentType ActiveEquipmentState;

};
