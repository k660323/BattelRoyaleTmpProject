// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGGA_UnActiveEquipment.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/PUBGItemInstance.h"
#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "System/PUBGAssetManager.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_UnActiveEquipment)

UPUBGGA_UnActiveEquipment::UPUBGGA_UnActiveEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_UnActiveEquipment);
	CancelAbilitiesWithTag.AddTag(PUBGGameplayTags::Ability_ActiveEquipment);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Weapon_Switching);

	// 로컬 예측 (로직 및 실행 타이밍)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 보안정책 클라 또는 서버 (어빌리티 종료 조건)
	//NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UPUBGGA_UnActiveEquipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APUBGBaseCharacter* Character= GetBaseCharacterFromActorInfo();
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

	EActiveWeaponEquipmentType CurActiveWeaponEquipmentType = ActiveEquipmentManager->GetCurActiveWeaponEquipmentType();
	if (CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Unarmed || CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Count)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	FPUBGActiveEquipmentEntry& Entry = ActiveEquipmentManager->GetCurActiveEquipmentEntry();
	UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

	if (ItemInstance == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	const UPUBGItemFragment_Attachment* Attachment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo();
	TSoftObjectPtr<UAnimMontage> UnEquipMontageSOP = Attachment->GetUnActiveEquipmentMontageByStateTag(ASC);

	UAnimMontage* UnEquipMontage = UPUBGAssetManager::Get().GetAssetByPath(UnEquipMontageSOP);
	if (UnEquipMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UpperBody"), UnEquipMontage, 1.0f, NAME_None, true, 1.0f, 0.f, false);

		if (HasAuthority(&ActivationInfo))
		{
			PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::OnEndMontage);
			PlayMontageAndWait->OnBlendOut.AddDynamic(this, &ThisClass::OnEndMontage);
			PlayMontageAndWait->ReadyForActivation();
		}
		else
		{
			PlayMontageAndWait->ReadyForActivation();
		}
	}
}

void UPUBGGA_UnActiveEquipment::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPUBGGA_UnActiveEquipment::OnEndMontage()
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

	if (HasAuthority(&CurrentActivationInfo))
	{
		ActiveEquipmentManager->ChangeEquipState(EActiveWeaponEquipmentType::Unarmed);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
