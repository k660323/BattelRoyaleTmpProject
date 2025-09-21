// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PUBGDefine.h"
#include "PUBGGameplayAbility.generated.h"

class UPUBGAbilitySystemComponent;
class APUBGBaseCharacter;
class AController;

UENUM(BlueprintType)
enum class EPUBGAbilityActivationPolicy : uint8
{
	Manual,	// 수동 (직접 호출 및 GameplayEvent)
	OnSpawn, // 어빌리티 할당시 어빌리티 실행
	OnTriggered, // 키 입력시 어빌리티 실행
	OnHold, // 키 홀딩시 어빌리티 실행
	OnGiven // 어빌리티 할당시 실행, 어빌리티 종료시 해당 어빌리티 제거
};

UENUM(BlueprintType)
enum class EPUBGAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

/** Failure reason that can be used to play an animation montage when a failure occurs */
USTRUCT(BlueprintType)
struct FPUBGAbilityMontageFailureMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	EPUBGAbilityActivationPolicy GetActivationPolicy() const;

	UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	EPUBGAbilityActivationGroup GetActivationGroup() const;

	// Returns true if the requested activation group is a valid transition.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "PUBG|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool CanChangeActivationGroup(EPUBGAbilityActivationGroup NewGroup) const;

	// Tries to change the activation group.  Returns true if it successfully changed.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "PUBG|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool ChangeActivationGroup(EPUBGAbilityActivationGroup NewGroup);

	//// Sets the ability's camera mode.
	//UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	//void SetCameraMode(TSubclassOf<UPUBGCameraMode> CameraMode);

	//// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	//UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	//void ClearCameraMode();

	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		ScriptOnAbilityFailedToActivate(FailedReason);
	}
protected:
	// Called when the ability fails to activate
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Called when the ability fails to activate
	UFUNCTION(BlueprintImplementableEvent)
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;


	// ability 인터페이스
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	// --------

	UFUNCTION(BlueprintPure, Category = "PUBG|Ability")
	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "PUBG|Ability")
	APUBGBaseCharacter* GetBaseCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	void GetMovementDirection(EPUBGDirection& OutDirection, FVector& OutMovementVector);

protected:
	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "PUBG|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Acotr", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EPUBGSuccessType& OutSuccessType);

	UFUNCTION(BlueprintCallable, Category = "PUBG|Ability")
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults);

protected:
	UFUNCTION(BlueprintCallable)
	void FlushPressedKey();

	UFUNCTION(BlueprintCallable)
	void FlushPressedInput(UInputAction* InputAction);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PUBG|Ability Activation")
	EPUBGAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PUBG|Ability Activation")
	EPUBGAbilityActivationGroup ActivationGroup;

	// Map of failure tags to simple error messages
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;

	// Map of failure tags to anim montages that should be played with them
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;
};
