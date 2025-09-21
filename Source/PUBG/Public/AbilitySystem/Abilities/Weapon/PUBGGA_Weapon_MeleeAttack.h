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
	// NotifyState���� Trace�� ���� �ǰݵ� ��� ����� ȣ��
	UFUNCTION()
	void OnTargetDataReady(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageEventTriggered(FGameplayEventData Payload);

	// ��Ÿ�� ����� �������� �����Ƽ ����
	UFUNCTION()
	void OnMontageFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category ="PUBG|MeleeAttack")
	TObjectPtr<UAnimMontage> AttackMontage;
};
