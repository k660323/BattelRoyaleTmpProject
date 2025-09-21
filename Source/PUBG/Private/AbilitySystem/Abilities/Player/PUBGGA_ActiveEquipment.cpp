// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGGA_ActiveEquipment.h"
#include "AbilitySystem/Abilities/Player/PUBGGA_SwapActiveEquipment.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers//PUBGEquipmentManagerComponent.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/PUBGItemInstance.h"
#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "System/PUBGAssetManager.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_ActiveEquipment)

UPUBGGA_ActiveEquipment::UPUBGGA_ActiveEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ActiveEquipmentState(EActiveWeaponEquipmentType::Count)
{

	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_ActiveEquipment);
	CancelAbilitiesWithTag.AddTag(PUBGGameplayTags::Ability_UnActiveEquipment);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Weapon_Switching);

	// ���� ���� (���� �� ���� Ÿ�̹�)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// ������å Ŭ�� �Ǵ� ���� (�����Ƽ ���� ����)
	//NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	// �� �����Ƽ�� Ȱ��ȭ �ɶ� ����� �±׵�
	TagsToCancelOnActivateAbility.AddTag(PUBGGameplayTags::Ability_SwapActiveEquipment);
}

void UPUBGGA_ActiveEquipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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

	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo();
	if (ASC == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(SwapAbilityClass);
	
	if (AbilitySpec)
	{
		if (UPUBGGA_SwapActiveEquipment* SwapActiveEquipmentAbility = CastChecked<UPUBGGA_SwapActiveEquipment>(AbilitySpec->GetPrimaryInstance()))
		{
			EActiveWeaponEquipmentType TargetActiveEquipmentState = SwapActiveEquipmentAbility->GetActiveEquipmentState();

			if (ActiveEquipmentState == TargetActiveEquipmentState)
			{
				CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
				return;
			}
		}
	}
	
	// �ش� �����Ƽ �±׸� ���� �����Ƽ ����
	ASC->CancelAbilities(&TagsToCancelOnActivateAbility, nullptr);

	// ���������� ��� ���
	EActiveWeaponEquipmentType CurActiveWeaponEquipmentType = ActiveEquipmentManager->GetCurActiveWeaponEquipmentType();
	if (ActiveEquipmentState == CurActiveWeaponEquipmentType)
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

	// Ȱ��ȭ �� ���� ���� ���� Ȯ��
	EEquipmentSlotType EEquipmentSlotType = UPUBGActiveEquipmentManagerComponent::ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(ActiveEquipmentState);
	FPUBGEquipmentEntry& Entry = EquipmentManager->GetEntry(EEquipmentSlotType);
	UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

	if (ItemInstance == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// �ٸ� ���� ����o
	// �ٸ� �����Ƽ ȣ��
	if (CurActiveWeaponEquipmentType != EActiveWeaponEquipmentType::Unarmed && CurActiveWeaponEquipmentType != EActiveWeaponEquipmentType::Count)
	{
		bSwap = true;
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	const UPUBGItemFragment_Attachment* Attachment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

	TSoftObjectPtr<UAnimMontage> EquipMontageSOP = Attachment->GetActiveEquipmentMontageByStateTag(ASC);

	UAnimMontage* EquipMontage = UPUBGAssetManager::Get().GetAssetByPath(EquipMontageSOP);
	if (EquipMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UpperBody"), EquipMontage, 1.0f, NAME_None, true, 1.0f, 0.f, false);

		if (HasAuthority(&CurrentActivationInfo))
		{
			PlayMontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::OnActiveEquipmentEndMontage);
			PlayMontageAndWait->OnBlendOut.AddDynamic(this, &ThisClass::OnActiveEquipmentEndMontage);
			PlayMontageAndWait->ReadyForActivation();
		}
		else
		{
			PlayMontageAndWait->ReadyForActivation();
		}
	}
}

void UPUBGGA_ActiveEquipment::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Ŭ���̾�Ʈ�� �ݹ� ȣ��
	if (HasAuthority(&ActivationInfo) == false)
	{
		if (bSwap && bWasCancelled)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					// ���� Ʈ���� �ߵ�
					FGameplayEventData EventData;
					EventData.EventMagnitude = (int)ActiveEquipmentState;
					SendGameplayEvent(PUBGGameplayTags::GameplayEvent_ActiveEquipment_Swap, EventData);
				}, 0.1f, false);
		}
	}
	bSwap = false;
}

void UPUBGGA_ActiveEquipment::OnActiveEquipmentEndMontage()
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
