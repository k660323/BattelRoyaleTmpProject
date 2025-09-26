// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_AimModeSwitcher.generated.h"

class UPUBGAbilityTask_WaitForTick;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_AimModeSwitcher : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_AimModeSwitcher(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnTick(float DeltaTime);
	
	UFUNCTION()
	void OnInputRelease(float TimeHeld);

private:
	UPROPERTY()
	bool bSwitch;

	UPROPERTY()
	float CurWaitTime;

	UPROPERTY(EditDefaultsOnly)
	float ThresHold;

	UPROPERTY()
	UPUBGAbilityTask_WaitForTick* WaitForTick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> ScopeAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> ADSAbilityClass;
};
