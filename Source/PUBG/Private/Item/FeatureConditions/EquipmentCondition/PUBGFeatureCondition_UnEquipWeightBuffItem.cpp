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

// Object : 탈착할 아이템, Object2 : 장착할 아이템
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

	// 현재 가방의 Effect 가져온다.
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

	

	// 현재 가방 무게
	float CurrentCapacityWeight = InventoryManager->GetCurrentCapacityWeight();
	// 현재 최대 가방 무게
	float TotalMaxCapacityWeight = InventoryManager->GetTotalAcceptableWeight();

	// 새로 장착될 아이템 버프
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

	// 가방 아이템이 적용시킨 Effect에서 SetByCaller_MaxCapacityWeight로 적용된 값을 가져온다.
	const FGameplayEffectSpec& Spec = ActiveGameplayEffect->Spec;
	float CurAddtiveWeight = Spec.GetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_AdditionalMaxCapacityWeight);

	// 해당값을 빼도 현재 무게보다 같거나 크면 장비를 해제할 수 있다.
	return CurrentCapacityWeight <= (TotalMaxCapacityWeight - CurAddtiveWeight + PostItemAddtiveWeight);
}
