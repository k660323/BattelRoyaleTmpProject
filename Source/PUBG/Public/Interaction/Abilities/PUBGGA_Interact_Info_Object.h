// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Abilities/PUBGGA_Interact_Info.h"
#include "PUBGGA_Interact_Info_Object.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PUBG_API UPUBGGA_Interact_Info_Object : public UPUBGGA_Interact_Info
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Interact_Info_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ability 인터페이스
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// --------

protected:
	UFUNCTION()
	void OnInvalidInteraction();

protected:
	UPROPERTY(EditDefaultsOnly, Category ="PUBG|Interaction")
	float AcceptanceAngle = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Interaction")
	float AcceptanceDistance = 100.0f;
};

