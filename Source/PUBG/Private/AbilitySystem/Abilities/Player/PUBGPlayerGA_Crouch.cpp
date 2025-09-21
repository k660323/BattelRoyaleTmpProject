// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGPlayerGA_Crouch.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitForTick.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utils/PUBGFunctionLibrary.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_TogglePressed.h"
#include "PUBGGameplayTags.h"
#include "Components/TimelineComponent.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerGA_Crouch)

UPUBGPlayerGA_Crouch::UPUBGPlayerGA_Crouch(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;

}

void UPUBGPlayerGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 쿨타임 확인
	if (K2_CheckAbilityCooldown() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CanCrouch() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();

	if (PlayerCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// 쿨타임 적용 (강제)
	if (CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	PlayerCharacter->Crouch();

	if (UPUBGAbilityTask_TogglePressed* TogglePressed = UPUBGAbilityTask_TogglePressed::TogglePressed(this))
	{
		TogglePressed->OnTogglePressed.AddDynamic(this, &ThisClass::OnTogglePressed);
		TogglePressed->OnToggleReleaseCondition.BindDynamic(this, &ThisClass::OnToggleReleaseCondition);
		TogglePressed->ReadyForActivation();
	}

	if (UPUBGAbilityTask_WaitForTick* TickTask = UPUBGAbilityTask_WaitForTick::WaitForTick(this))
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnTick);
		TickTask->ReadyForActivation();
	}

	if (AActor* Actor = GetAvatarActorFromActorInfo())
	{
		if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Actor->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
		{
			ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
		}
	}
}

void UPUBGPlayerGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		PlayerCharacter->UnCrouch();
	}

	if (bWasCancelled == false)
	{
		SendGameplayEvent(PUBGGameplayTags::GameplayEvent_Stand, FGameplayEventData());
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


//FActiveGameplayEffectHandle UPUBGPlayerGA_Crouch::ApplyGameplayEffectToOwnerCustom(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const UGameplayEffect* GameplayEffect, float GameplayEffectLevel, int32 Stacks) const
//{
//	if (GameplayEffect && (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)))
//	{
//		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, GameplayEffect->GetClass(), GameplayEffectLevel);
//		if (SpecHandle.IsValid())
//		{
//			SpecHandle.Data->SetStackCount(Stacks);
//			const FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
//			if (Spec)
//			{
//				SpecHandle.Data->SetSetByCallerMagnitude(PUBGGameplayTags::Cooldown_Status_Crouch, 10.0f);
//			}
//
//			return ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
//		}
//	}
//
//	// We cannot apply GameplayEffects in this context. Return an empty handle.
//	return FActiveGameplayEffectHandle();
//}

bool UPUBGPlayerGA_Crouch::CanCrouch()
{
	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (PlayerCharacter == nullptr)
		return false;

	UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();
	if (CharacterMovement == nullptr || CharacterMovement->IsFalling())
		return false;

	return true;
}

void UPUBGPlayerGA_Crouch::OnTogglePressed()
{
	// 강제 종료
	if (IsActive())
	{
		// 쿨타임 적용 (강제)
		CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

bool UPUBGPlayerGA_Crouch::OnToggleReleaseCondition()
{
	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();

	if (PlayerCharacter)
	{
		return (UPUBGFunctionLibrary::NativeDoesActorHaveTag(PlayerCharacter, PUBGGameplayTags::Status_StateTransform) == false);
	}

	return true;
}

void UPUBGPlayerGA_Crouch::OnTick(float DeltaTime)
{
	if (CanCrouch() == false)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
