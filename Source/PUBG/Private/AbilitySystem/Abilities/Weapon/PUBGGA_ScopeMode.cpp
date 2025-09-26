// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Weapon/PUBGGA_ScopeMode.h"
#include "GameFramework/Controller.h"
#include "System/PUBGAssetManager.h"
#include "Camera/CameraSpringArmTimelineComponent.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "PUBGGameplayTags.h"

UPUBGGA_ScopeMode::UPUBGGA_ScopeMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_ScopeMode);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Aiming);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Aim_Scope);
}

bool UPUBGGA_ScopeMode::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UPUBGGA_ScopeMode::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APUBGPlayerCharacter* Character = Cast<APUBGPlayerCharacter>(GetBaseCharacterFromActorInfo());
	if (Character == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (Character->IsFPSMode() == false)
	{
		Character->SetFPSMode(true);
	}

	if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
	{
		ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
	}
}

void UPUBGGA_ScopeMode::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (APUBGPlayerCharacter* Character = Cast<APUBGPlayerCharacter>(GetBaseCharacterFromActorInfo()))
	{
		if (Character->IsFPSMode())
		{
			Character->SetFPSMode(false);
		}

		if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
		{
			ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
		}
	}
}
