// Kakanu Portfolio Project

#pragma once

#include "Interaction/Abilities/PUBGGA_Interact_Info.h"
#include "PUBGGA_Interact_Active.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Interact_Active : public UPUBGGA_Interact_Info
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Interact_Active(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UFUNCTION()
	void TriggerInteraction();

	UFUNCTION()
	void OnInputPrssed(float TimeWaited);

	UFUNCTION()
	void OnDurationEnded();

	UFUNCTION()
	void OnNetSync();
	
	UFUNCTION()
	void OnInvalidInteraction();

protected:
	UPROPERTY(EditDefaultsOnly, Category="PUBG|Interaction")
	TObjectPtr<UInputAction> MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category ="PUBG|Interaction")
	float AcceptanceAngle = 65.0f;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Interaction")
	float AcceptanceDistance = 10.f;

	FTimerHandle InteractionTimerHandle;

	UPROPERTY()
	float CachedInteractionRange = 0.0f;
};
