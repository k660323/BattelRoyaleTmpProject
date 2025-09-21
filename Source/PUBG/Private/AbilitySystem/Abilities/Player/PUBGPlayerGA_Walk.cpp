// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGPlayerGA_Walk.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Utils/PUBGFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerGA_Walk)

UPUBGPlayerGA_Walk::UPUBGPlayerGA_Walk(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnHold;
}

void UPUBGPlayerGA_Walk::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// (클라, 서버) 둘다 실행
	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (PlayerCharacter == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (MovementComponent == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnReInputRelease);
		WaitInputRelease->ReadyForActivation();
	}

}

void UPUBGPlayerGA_Walk::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UPUBGPlayerGA_Walk::OnReInputRelease(float Time)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
