// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGGA_SwapActiveEquipment.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Item/Managers//PUBGEquipmentManagerComponent.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/PUBGItemInstance.h"
#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "Characters/PUBGBaseCharacter.h"
#include "System/PUBGAssetManager.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_SwapActiveEquipment)

UPUBGGA_SwapActiveEquipment::UPUBGGA_SwapActiveEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_SwapActiveEquipment);
	CancelAbilitiesWithTag.AddTag(PUBGGameplayTags::Ability_ActiveEquipment);
	CancelAbilitiesWithTag.AddTag(PUBGGameplayTags::Ability_UnActiveEquipment);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Weapon_Switching);
	
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_ActiveEquipment_Swap;
		AbilityTriggers.Add(TriggerData);
	}

	// 로컬 예측 (로직 및 실행 타이밍)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	ActiveEquipmentState = EActiveWeaponEquipmentType::Count;
}

void UPUBGGA_SwapActiveEquipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	ActiveEquipmentState = (EActiveWeaponEquipmentType)((int)TriggerEventData->EventMagnitude);

	if (ActiveEquipmentState == EActiveWeaponEquipmentType::Unarmed || ActiveEquipmentState == EActiveWeaponEquipmentType::Count)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UnActiveEquipment();
}

void UPUBGGA_SwapActiveEquipment::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ActiveEquipmentState = EActiveWeaponEquipmentType::Count;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPUBGGA_SwapActiveEquipment::UnActiveEquipment()
{
	APUBGBaseCharacter* Character = GetBaseCharacterFromActorInfo();
	if (Character == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	if (ActiveEquipmentManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	EActiveWeaponEquipmentType CurActiveWeaponEquipmentType = ActiveEquipmentManager->GetCurActiveWeaponEquipmentType();
	if (CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Unarmed || CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Count)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	FPUBGActiveEquipmentEntry& Entry = ActiveEquipmentManager->GetCurActiveEquipmentEntry();
	UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

	if (ItemInstance == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	const UPUBGItemFragment_Attachment* Attachment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo();
	TSoftObjectPtr<UAnimMontage> UnEquipMontageSOP = Attachment->GetUnActiveEquipmentMontageByStateTag(ASC);

	UAnimMontage* UnEquipMontage = UPUBGAssetManager::Get().GetAssetByPath(UnEquipMontageSOP);
	if (UnEquipMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UpperBody"), UnEquipMontage, 1.0f, NAME_None, true, 1.0f, 0.f, false);

		PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::OnEndUnActiveEquipmentMontage);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this, &ThisClass::OnEndUnActiveEquipmentMontage);
		PlayMontageAndWait->ReadyForActivation();
	}
}

void UPUBGGA_SwapActiveEquipment::ActiveEquipment()
{
	APUBGBaseCharacter* Character = GetBaseCharacterFromActorInfo();
	if (Character == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	if (ActiveEquipmentManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UPUBGEquipmentManagerComponent* EquipmentManager = Character->FindComponentByClass<UPUBGEquipmentManagerComponent>();
	if (EquipmentManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// 활성화 할 무기 존재 여부 확인
	EEquipmentSlotType EEquipmentSlotType = UPUBGActiveEquipmentManagerComponent::ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(ActiveEquipmentState);
	FPUBGEquipmentEntry& Entry = EquipmentManager->GetEntry(EEquipmentSlotType);
	UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

	if (ItemInstance == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	const UPUBGItemFragment_Attachment* Attachment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo();
	TSoftObjectPtr<UAnimMontage> EquipMontageSOP = Attachment->GetActiveEquipmentMontageByStateTag(ASC);

	UAnimMontage* EquipMontage = UPUBGAssetManager::Get().GetAssetByPath(EquipMontageSOP);
	if (EquipMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UpperBody"), EquipMontage, 1.0f, NAME_None, true, 1.0f, 0.f, false);
		if (HasAuthority(&CurrentActivationInfo))
		{
			PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::OnEndActiveEquipmentMontage);
			PlayMontageAndWait->OnBlendOut.AddDynamic(this, &ThisClass::OnEndActiveEquipmentMontage);
		}
		PlayMontageAndWait->ReadyForActivation();
	}
}

EActiveWeaponEquipmentType UPUBGGA_SwapActiveEquipment::GetActiveEquipmentState() const
{
	return ActiveEquipmentState;
}

void UPUBGGA_SwapActiveEquipment::OnEndUnActiveEquipmentMontage()
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		APUBGBaseCharacter* Character = GetBaseCharacterFromActorInfo();
		if (Character == nullptr)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}

		UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
		if (ActiveEquipmentManager == nullptr)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}

		if (ActiveEquipmentManager->GetCurActiveWeaponEquipmentType() == EActiveWeaponEquipmentType::Unarmed || ActiveEquipmentManager->GetCurActiveWeaponEquipmentType() == EActiveWeaponEquipmentType::Count)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}

		ActiveEquipmentManager->ChangeEquipState(EActiveWeaponEquipmentType::Unarmed);
	}

	if (UAbilityTask_NetworkSyncPoint* NetworkSyncPoint = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyClientWait))
	{
		NetworkSyncPoint->OnSync.AddDynamic(this, &ThisClass::OnUnActiveEquipmentNetSync);
		NetworkSyncPoint->ReadyForActivation();
	}
}

void UPUBGGA_SwapActiveEquipment::OnUnActiveEquipmentNetSync()
{
	ActiveEquipment();
}

void UPUBGGA_SwapActiveEquipment::OnEndActiveEquipmentMontage()
{
	APUBGBaseCharacter* Character = GetBaseCharacterFromActorInfo();
	if (Character == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	if (ActiveEquipmentManager == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	ActiveEquipmentManager->ChangeEquipState(ActiveEquipmentState);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}