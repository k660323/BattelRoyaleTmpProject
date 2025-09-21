// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/PUBGGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "PUBGAbilitySystemComponent.generated.h"

class UPUBGAbilityTagRelationshipMapping;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UPUBGAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	typedef TFunctionRef<bool(const UPUBGGameplayAbility* PUBGAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);


	void OnAbilityTagStarted(const FGameplayTag& InInputTag);
	void OnAbilityTagPressed(const FGameplayTag& InInputTag);
	void OnAbilityTagReleased(const FGameplayTag& InInputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EPUBGAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EPUBGAbilityActivationGroup Group, UPUBGGameplayAbility* PUBGAbility);
	void RemoveAbilityFromActivationGroup(EPUBGAbilityActivationGroup Group, UPUBGGameplayAbility* PUBGAbility);
	void CancelActivationGroupAbilities(EPUBGAbilityActivationGroup Group, UPUBGGameplayAbility* IgnorePUBGAbility, bool bReplicateCancelAbility);
	

	// Uses a gameplay effect to add the specified dynamic granted tag.
	UFUNCTION(BlueprintCallable)
	void AddDynamicTagGameplayEffect(FGameplayTag Tag);

	FActiveGameplayEffectHandle AddDynamicTagsGameplayEffect(TArray<FGameplayTag> Tags);

	// Removes all active instances of the gameplay effect that was used to add the specified dynamic granted tag.
	UFUNCTION(BlueprintCallable)
	void RemoveDynamicTagGameplayEffect(FGameplayTag Tag);

	/** Gets the ability target data associated with the given ability handle and activation info */
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(UPUBGAbilityTagRelationshipMapping* NewMapping);

	/** Looks at ability tags and gathers additional required and blocking tags */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	bool HasChildTagOfParent(const FGameplayTag& InParentTag);

protected:
	virtual void AbilitySecInputStarted(FGameplayAbilitySpec& Spec);
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;

	/** Notify client that an ability failed to activate */
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

protected:
	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	TObjectPtr<UPUBGAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Handles to abilities that had their input started this frame.
	TArray<FGameplayAbilitySpecHandle> InputStartedSpecHandles;

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Number of abilities running in each activation group.
	int32 ActivationGroupCounts[(uint8)EPUBGAbilityActivationGroup::MAX];
};
