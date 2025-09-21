// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "PUBGPlayerGA_Prone.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGA_Prone : public UPUBGPlayerGameplayAbility
{
	GENERATED_BODY()
public:
	UPUBGPlayerGA_Prone(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	bool CanProne();

	UFUNCTION()
	void OnTogglePressed();

	UFUNCTION()
	bool OnToggleReleaseCondition();
};
