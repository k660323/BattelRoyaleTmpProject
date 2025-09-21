// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_EquipBackpack.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "PUBGGameplayTags.h"

UPUBGGA_EquipBackpack::UPUBGGA_EquipBackpack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnSpawn;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Equip_Backpack);
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
}

void UPUBGGA_EquipBackpack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ReplaceWeaponSocket();
}

void UPUBGGA_EquipBackpack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ReplaceWeaponSocket();
}

void UPUBGGA_EquipBackpack::ReplaceWeaponSocket()
{
	APUBGBaseCharacter* Character = GetBaseCharacterFromActorInfo();
	if (Character == nullptr)
	{
		return;
	}

	UPUBGEquipmentManagerComponent* EquipmentManager = Character->FindComponentByClass<UPUBGEquipmentManagerComponent>();
	if (EquipmentManager == nullptr)
	{
		return;
	}

	EquipmentManager->ReplaceWeaponEquipmentSocket();
}
