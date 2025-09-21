// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "PUBGPlayerGA_Crouch.generated.h"

class UTimelineComponent;
class UCurveFloat;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGA_Crouch : public UPUBGPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGPlayerGA_Crouch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
public:
	bool CanCrouch();

protected:
	UFUNCTION()
	void OnTogglePressed();

	UFUNCTION()
	bool OnToggleReleaseCondition();

	UFUNCTION()
	void OnTick(float DeltaTime);
};
