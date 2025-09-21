// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_Interact.generated.h"

USTRUCT(BlueprintType)
struct FPUBGInteractionMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRefresh = false;

	UPROPERTY(BlueprintReadWrite)
	bool bSwitchActive = false;

	UPROPERTY(BlueprintReadWrite)
	FPUBGInteractionInfo InteractionInfo = FPUBGInteractionInfo();
};


USTRUCT(BlueprintType)
struct FPUBGInteractionsMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRefresh = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<FPUBGInteractionInfo> InteractionInfos;
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Interact : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FPUBGInteractionInfo>& InteractionInfos);

	UFUNCTION(BlueprintCallable)
	void UpdateNearInteractions(const TArray<FPUBGInteractionInfo>& InteractionInfos);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

private:
	void WaitInputStart();

	UFUNCTION()
	void OnInputStart();

public:
	float GetInteractionTraceRange();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FPUBGInteractionInfo> CurrentInteractionInfos;

	UPROPERTY(BlueprintReadWrite)
	TArray<FPUBGInteractionInfo> CurrentNearInteractionInfos;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Interaction")
	float InteractionTraceRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Interaction")
	float InteractionLineTraceRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Interaction")
	float InteractionSphereTraceRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="PUBG|Interaction")
	bool bShowTraceDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Interaction")
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
