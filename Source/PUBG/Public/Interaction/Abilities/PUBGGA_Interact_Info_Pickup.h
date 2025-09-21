// Kakanu Portfolio Project

#pragma once

#include "Interaction/Abilities/PUBGGA_Interact_Info_Object.h"
#include "PUBGGA_Interact_Info_Pickup.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Interact_Info_Pickup : public UPUBGGA_Interact_Info_Object
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Interact_Info_Pickup(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ability 인터페이스
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// --------
};
