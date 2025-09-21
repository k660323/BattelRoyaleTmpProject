// Kakanu Portfolio Project

#pragma once

#include "GameplayEffect.h"
#include "AbilitySystem/Abilities/Weapon/PUBGGA_Weapon_MeleeAttack.h"
#include "PUBGGA_Weapon_MeleeCombo.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UPUBGAbilityTask_WaitInputStart;
class UAbilityTask_WaitGameplayEvent;
class APUBGEquipmentBase;
class UPUBGItemInstance;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Weapon_MeleeCombo : public UPUBGGA_Weapon_MeleeAttack
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void HandleMontageEvent(FGameplayEventData Payload) override;

	void InputStart();

private:
	UFUNCTION()
	void OnAllowInputPress(FGameplayEventData Payload);

	UFUNCTION()
	void OnInputStart();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Melee Combo")
	TSubclassOf<UPUBGGA_Weapon_MeleeCombo> NextAbilityClass;

private:
	UPROPERTY()
	bool bAllowInputPress;

	UPROPERTY()
	bool bInputStart;
};
