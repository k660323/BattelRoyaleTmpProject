// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/Weapon/PUBGGA_Weapon_Melee.h"
#include "PUBGGA_Weapon_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Weapon_MeleeAttack : public UPUBGGA_Weapon_Melee
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Weapon_MeleeAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void HandleMontageEvent(FGameplayEventData Payload);

private:
	// NotifyState에서 Trace로 통해 피격된 대상 존재시 호출
	UFUNCTION()
	void OnTargetDataReady(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageEventTriggered(FGameplayEventData Payload);

	// 몽타주 종료시 서버에서 어빌리티 종료
	UFUNCTION()
	void OnMontageFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category ="PUBG|MeleeAttack")
	TObjectPtr<UAnimMontage> AttackMontage;
};
