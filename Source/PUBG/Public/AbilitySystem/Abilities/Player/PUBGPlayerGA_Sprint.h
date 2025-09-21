// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "PUBGPlayerGA_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGA_Sprint : public UPUBGPlayerGameplayAbility
{
	GENERATED_BODY()
public:
	UPUBGPlayerGA_Sprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// ��ƽ���� ����,Ŭ�� ���� ������� ������ üũ�Ͽ� �չ����� �ƴϸ�
	// �޸��� �����Ƽ ����
	UFUNCTION()
	void OnTick(float DeltaTime);

	UFUNCTION()
	void OnReInputRelease(float Time);

	//UFUNCTION()
	//void OnSprintCommitTick();

protected:
	//UPROPERTY(EditDefaultsOnly, Category = "PUBG|Sprint Active")
	//TSubclassOf<UGameplayEffect> RecoveryBlockEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Sprint Active")
	float ForwardThreshold = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Sprint Active")
	float CommitInterval = 0.1f;

private:
	// FTimerHandle SprintTimerHandle;
};
