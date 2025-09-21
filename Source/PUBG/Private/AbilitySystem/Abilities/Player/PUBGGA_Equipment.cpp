// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGGA_Equipment.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/PUBGItemInstance.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Actors/PUBGEquipmentBase.h"
#include "AbilitySystem/Attributes/PUBGCombatSet.h"
#include "System/PUBGAssetManager.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Equipment)

UPUBGGA_Equipment::UPUBGGA_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bServerRespectsRemoteAbilityCancellation = false;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

bool UPUBGGA_Equipment::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags) == false)
	{
		return false;
	}

	APUBGBaseCharacter* Character = Cast<APUBGBaseCharacter>(ActorInfo->AvatarActor.Get());
	if (Character == nullptr)
	{
		return false;
	}

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	if (ActiveEquipmentManager == nullptr)
	{
		return false;
	}

	if (ActiveEquipmentInfo.EquipmentType == EEquipmentType::Count)
	{
		return false;
	}

	if (ActiveEquipmentInfo.EquipmentType == EEquipmentType::Armor)
	{
		UPUBGItemInstance* ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredArmorType);
		if (ItemInstance == nullptr)
		{
			return false;
		}
	}
	else if (ActiveEquipmentInfo.EquipmentType == EEquipmentType::Weapon)
	{
		UPUBGItemInstance* ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredWeaponType);
		if (ItemInstance == nullptr)
		{
			return false;
		}

		const UPUBGItemFragment_Weapon* WeaponFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Weapon>();
		if (WeaponFragment == nullptr)
		{
			return false;
		}

		if (WeaponFragment->WeaponType != ActiveEquipmentInfo.RequiredWeaponType)
		{
			return false;
		}
	}
	else if (ActiveEquipmentInfo.EquipmentType == EEquipmentType::Utility)
	{
		UPUBGItemInstance* ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredUtilityType);
		if (ItemInstance == nullptr)
		{
			return false;
		}
	}
	else if (ActiveEquipmentInfo.EquipmentType == EEquipmentType::Fashion)
	{
		UPUBGItemInstance* ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredFashionType);
		if (ItemInstance == nullptr)
		{
			return false;
		}
	}

	return true;
}

bool UPUBGGA_Equipment::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	bool Result = Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);

	if (Result)
	{
		const  TSubclassOf<UGameplayEffect> AttributeModifierGE = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("Attribute_Modifier"));
		check(AttributeModifierGE);

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(*AttributeModifierGE, 1.f, ContextHandle);

		// SetByCaller 값 0으로 초기화
		const TSharedPtr<FGameplayEffectSpec>& SpecData = SpecHandle.Data;

		for (const FGameplayModifierInfo& ModifierInfo : SpecData->Def->Modifiers)
		{
			SpecData->SetSetByCallerMagnitude(ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag, 0.0f);
		}

		for (const FStatSet& StatSet : StatSets)
		{
			SpecData->SetSetByCallerMagnitude(StatSet.StatTag, StatSet.Value);
		}

		GameplayEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
	
	return Result;
}


void UPUBGGA_Equipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APUBGBaseCharacter* Character = Cast<APUBGBaseCharacter>(ActorInfo->AvatarActor.Get());
	if (Character == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();

	if (ActiveEquipmentManager == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UPUBGItemInstance* ItemInstance = nullptr;

	switch (ActiveEquipmentInfo.EquipmentType)
	{
	case EEquipmentType::Armor:
		ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredArmorType);
		break;
	case EEquipmentType::Weapon:
		ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredWeaponType);
		ActiveEquipmentInfo.EquipmentActor = ActiveEquipmentManager->GetEntryEquipmentActorFromEquipmentManager(ActiveEquipmentInfo.RequiredWeaponType);
		break;
	case EEquipmentType::Utility:
		ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredUtilityType);
		break;
	case EEquipmentType::Fashion:
		ItemInstance = ActiveEquipmentManager->GetEquipItemInstance(ActiveEquipmentInfo.RequiredFashionType);
		break;
	}

	if ((ItemInstance == nullptr) || (ActiveEquipmentInfo.EquipmentType == EEquipmentType::Weapon && ActiveEquipmentInfo.EquipmentActor == nullptr))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	CachedAttackRate = DefaultAttackRate;
	if (UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo())
	{
		if (const UPUBGCombatSet* CombatSet = ASC->GetSet<UPUBGCombatSet>())
		{
			float AttackSpeedPercent = CombatSet->GetAttackSpeedPercent();
			CachedAttackRate = DefaultAttackRate + (DefaultAttackRate * AttackSpeedPercent);
		}
	}
}

void UPUBGGA_Equipment::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&ActivationInfo))
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		ASC->RemoveActiveGameplayEffect(GameplayEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

APUBGEquipmentBase* UPUBGGA_Equipment::GetEquipmentActor() const
{
	if (APUBGEquipmentBase* EquipmentActor = ActiveEquipmentInfo.EquipmentActor.Get())
	{
		return EquipmentActor;
	}

	return nullptr;
}

UPUBGItemInstance* UPUBGGA_Equipment::GetEquipmentItemInstance(const APUBGEquipmentBase* InEquipmentActor) const
{
	if (InEquipmentActor == nullptr)
	{
		return nullptr;
	}

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = GetBaseCharacterFromActorInfo()->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	if (ActiveEquipmentManager == nullptr)
	{
		return nullptr;
	}

	if (ActiveEquipmentInfo.EquipmentActor == InEquipmentActor)
	{
		return ActiveEquipmentManager->GetEquipItemInstance(InEquipmentActor->GetEquipmentSlotType());
	}

	return nullptr;
}

float UPUBGGA_Equipment::GetEquipmentStatValue(FGameplayTag InStatTag, const APUBGEquipmentBase* InEquipmentActor) const
{
	if (InStatTag.IsValid() == false || InEquipmentActor == nullptr)
	{
		return 0.0f;
	}

	if (UPUBGItemInstance* ItemInstance = GetEquipmentItemInstance(InEquipmentActor))
	{
		return ItemInstance->GetStackCountByTag(InStatTag);
	}

	check(true);

	return 0.0f;
}
