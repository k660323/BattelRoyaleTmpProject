// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "PUBGPlayerGA_SprintCheck.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGA_SprintCheck : public UPUBGPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGPlayerGA_SprintCheck(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION(Server, Reliable)
	void Server_RequestSprint(EPUBGDirection Direction, FVector MovementVector);

private:
	void SendSprintActiveEvent(EPUBGDirection Direction, FVector MovementVector);

//protected:
//	UPROPERTY(EditDefaultsOnly, Category = "PUBG/Sprint Check")
//	FGameplayAttribute SprintCostAttribute;
};
