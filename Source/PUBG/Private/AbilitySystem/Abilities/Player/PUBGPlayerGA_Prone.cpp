// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGPlayerGA_Prone.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utils/PUBGFunctionLibrary.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_TogglePressed.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerGA_Prone)

UPUBGPlayerGA_Prone::UPUBGPlayerGA_Prone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;
}

void UPUBGPlayerGA_Prone::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ��Ÿ�� Ȯ��
	if (K2_CheckAbilityCooldown() == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CanProne() == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// (Ŭ��, ����) �Ѵ� ����
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

	// ��Ÿ�� ���� (����)
	if (CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (UPUBGAbilityTask_TogglePressed* TogglePressed = UPUBGAbilityTask_TogglePressed::TogglePressed(this))
	{
		TogglePressed->OnTogglePressed.AddDynamic(this, &ThisClass::OnTogglePressed);
		TogglePressed->OnToggleReleaseCondition.BindDynamic(this, &ThisClass::OnToggleReleaseCondition);
		TogglePressed->ReadyForActivation();
	}

	if (IsLocallyControlled())
	{
		PlayerCharacter->SetProneSpringArm(true);
	}

	if (AActor* Actor = GetAvatarActorFromActorInfo())
	{
		if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Actor->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
		{
			ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
		}
	}
}

void UPUBGPlayerGA_Prone::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsLocallyControlled())
	{
		if (APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
		{
			PlayerCharacter->SetProneSpringArm(false);
		}
	}

	if (bWasCancelled == false)
	{
		SendGameplayEvent(PUBGGameplayTags::GameplayEvent_Stand, FGameplayEventData());
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UPUBGPlayerGA_Prone::CanProne()
{
	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (PlayerCharacter == nullptr)
		return false;

	UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();
	if (CharacterMovement == nullptr || CharacterMovement->IsFalling())
		return false;

	return true;
}

void UPUBGPlayerGA_Prone::OnTogglePressed()
{
	// ���� ����
	if (IsActive())
	{
		// ��Ÿ�� ���� (����)
		CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

bool UPUBGPlayerGA_Prone::OnToggleReleaseCondition()
{
	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();

	if (PlayerCharacter)
	{
		return (UPUBGFunctionLibrary::NativeDoesActorHaveTag(PlayerCharacter, PUBGGameplayTags::Status_StateTransform) == false);
	}

	return true;
}
