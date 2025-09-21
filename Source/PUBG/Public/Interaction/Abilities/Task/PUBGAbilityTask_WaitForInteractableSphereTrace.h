#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/PUBGInteractionQuery.h"
#include "PUBGAbilityTask_WaitForInteractableSphereTrace.generated.h"

struct FPUBGInteractionInfo;
class IPUBGInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNearInteractableChanged, const TArray<FPUBGInteractionInfo>&, InteractionInfos);

UCLASS()
class PUBG_API UPUBGAbilityTask_WaitForInteractableSphereTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPUBGAbilityTask_WaitForInteractableSphereTrace(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPUBGAbilityTask_WaitForInteractableSphereTrace* WaitForInteractableSphereTrace(UGameplayAbility* OwningAbility, FPUBGInteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionRadius = 50.0f, float InteractionRate = 0.1f, bool bShowDebug = false);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void PerformTrace();

	void UpdateInteractionInfo(const TArray<TScriptInterface<IPUBGInteractable>>& Interactables);
	
public:
	FOnNearInteractableChanged NearInteractableChanged;

private:
	UPROPERTY()
	FTimerHandle TraceTimerHandle;
	
	UPROPERTY()
	TArray<FPUBGInteractionInfo> CurrentInteractionInfos;

private:
	UPROPERTY()
	FPUBGInteractionQuery InteractionQuery;
	ECollisionChannel TraceChannel = ECC_Visibility;
	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;
	float InteractionRadius = 50.0f;
	float InteractionRate = 0.1f;
	bool bShowDebug = false;

};

