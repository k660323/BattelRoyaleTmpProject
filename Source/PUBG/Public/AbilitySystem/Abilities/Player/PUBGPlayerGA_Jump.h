// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "PUBGPlayerGA_Jump.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGA_Jump : public UPUBGPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGPlayerGA_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(Server, Reliable)
	void Server_RequestJump();

protected:
	UFUNCTION()
	void StartJump();
};
