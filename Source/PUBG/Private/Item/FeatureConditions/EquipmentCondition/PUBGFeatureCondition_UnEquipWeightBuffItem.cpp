//// Kakanu Portfolio Project

#include "Item/FeatureConditions/EquipmentCondition/PUBGFeatureCondition_UnEquipWeightBuffItem.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/Managers/PUBGInventoryManagerComponent.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "PUBGGameplayTags.h"
#include "Item/PUBGItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGFeatureCondition_UnEquipWeightBuffItem)

UPUBGFeatureCondition_UnEquipWeightBuffItem::UPUBGFeatureCondition_UnEquipWeightBuffItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

// Object : Ż���� ������, Object2 : ������ ������
bool UPUBGFeatureCondition_UnEquipWeightBuffItem::CanUseFeature(UObject* Owner, UPUBGItemInstance* UnequipItem, UPUBGItemInstance* EquipItem) const
{
	if (Owner == nullptr)
	{
		return true;
	}

	APUBGBaseCharacter* Character = Cast<APUBGBaseCharacter>(Owner);
	if (Character == nullptr)
	{
		return true;
	}

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	
	if (ASC == nullptr)
	{
		return true;
	}

	UPUBGActiveEquipmentManagerComponent* EquipmentManager = Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	if (EquipmentManager == nullptr)
	{
		return true;
	}

	UPUBGInventoryManagerComponent* InventoryManager = Character->FindComponentByClass<UPUBGInventoryManagerComponent>();
	if (InventoryManager == nullptr)
	{
		return true;
	}

	FPUBGActiveEquipmentEntry& Entry = EquipmentManager->GetEntry(EEquipmentSlotType::Backpack);
	if (Entry.GetItemInstance() == nullptr)
	{
		return true;
	}

	// ���� ������ Effect �����´�.
	const FActiveGameplayEffectHandle& BaseStatHandle = Entry.GetBaseStatHandle();
	if (BaseStatHandle.IsValid() == false)
	{
		return true;
	}

	const FActiveGameplayEffect* ActiveGameplayEffect = ASC->GetActiveGameplayEffect(BaseStatHandle);
	if (ActiveGameplayEffect == nullptr)
	{
		return true;
	}

	

	// ���� ���� ����
	float CurrentCapacityWeight = InventoryManager->GetCurrentCapacityWeight();
	// ���� �ִ� ���� ����
	float TotalMaxCapacityWeight = InventoryManager->GetTotalAcceptableWeight();

	// ���� ������ ������ ����
	float PostItemAddtiveWeight = 0.0f;
	if (EquipItem)
	{
		if (UPUBGItemInstance* ItemInstance = Cast<UPUBGItemInstance>(EquipItem))
		{
			if (ItemInstance->HasStatTag(PUBGGameplayTags::SetByCaller_AdditionalMaxCapacityWeight))
			{
				PostItemAddtiveWeight = ItemInstance->GetStackCountByTag(PUBGGameplayTags::SetByCaller_AdditionalMaxCapacityWeight);
			}
		}
	}

	// ���� �������� �����Ų Effect���� SetByCaller_MaxCapacityWeight�� ����� ���� �����´�.
	const FGameplayEffectSpec& Spec = ActiveGameplayEffect->Spec;
	float CurAddtiveWeight = Spec.GetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_AdditionalMaxCapacityWeight);

	// �ش簪�� ���� ���� ���Ժ��� ���ų� ũ�� ��� ������ �� �ִ�.
	return CurrentCapacityWeight <= (TotalMaxCapacityWeight - CurAddtiveWeight + PostItemAddtiveWeight);
}
