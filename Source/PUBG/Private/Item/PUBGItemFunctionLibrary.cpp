// Kakanu Portfolio Project


#include "Item/PUBGItemFunctionLibrary.h"
#include "Item/PUBGItemTemplate.h"
#include "Item/PUBGItemInstance.h"
#include "Actors/PUBGPickupableItemBase.h"
#include "Data/PUBGItemData.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "GameMode/PUBGGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFunctionLibrary)

UPUBGItemInstance* UPUBGItemFunctionLibrary::NativeCreateItemInstance(int32 ItemTemplateID)
{
	UPUBGItemInstance* ItemInstance = NewObject<UPUBGItemInstance>();
	ItemInstance->Init(ItemTemplateID);

	return ItemInstance;
}

UPUBGItemInstance* UPUBGItemFunctionLibrary::BP_CreateItemInstance(int32 ItemTemplateID)
{
	return NativeCreateItemInstance(ItemTemplateID);
}

APUBGPickupableItemBase* UPUBGItemFunctionLibrary::NativeCreatePickupableItemBase(UObject* WorldContextObject, int32 ItemTemplateID, int32 ItemCount)
{
	if (WorldContextObject == nullptr)
	{
		return nullptr;
	}
	
	UPUBGItemInstance* ItemInstance = NativeCreateItemInstance(ItemTemplateID);

	FPUBGPickupInstance PickupInstance;
	PickupInstance.ItemInstance = ItemInstance;
	PickupInstance.ItemCount = ItemCount;

	APUBGPickupableItemBase* PickupableItemBase = Cast<APUBGPickupableItemBase>(WorldContextObject->GetWorld()->SpawnActor(APUBGPickupableItemBase::StaticClass()));
	if (PickupableItemBase->GetPickupInfo().ItemInstance)
	{
		PickupableItemBase->RemoveReplicatedSubObject(PickupableItemBase->GetPickupInfo().ItemInstance);
	}
	if (PickupableItemBase->IsUsingRegisteredSubObjectList())
	{
		PickupableItemBase->AddReplicatedSubObject(PickupInstance.ItemInstance);
	}
	PickupableItemBase->SetPickupInfo(PickupInstance);

	return PickupableItemBase;
}

APUBGPickupableItemBase* UPUBGItemFunctionLibrary::BP_CreatePickupableItemBase(UObject* WorldContextObject, int32 ItemTemplateID, int32 ItemCount)
{
	return NativeCreatePickupableItemBase(WorldContextObject, ItemTemplateID, ItemCount);
}

void UPUBGItemFunctionLibrary::NativeEquipEquipItem(int32 ItemTemplateID, AActor* Actor)
{
	UPUBGActiveEquipmentManagerComponent* EquipmentManagerComponent = Actor->GetComponentByClass<UPUBGActiveEquipmentManagerComponent>();

	if (EquipmentManagerComponent == nullptr)
	{
		return;
	}

	UPUBGItemInstance* ItemInstance = NativeCreateItemInstance(ItemTemplateID);

	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();

	if (EquipableFragment == nullptr)
	{
		return;
	}

	if (const UPUBGItemFragment_Fashion* FashionFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Fashion>())
	{
		EFashionType FashionType = FashionFragment->FashionType;
		EEquipmentSlotType EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::FashionTypeConvertToEquipmentSlotType(FashionType);
		EquipmentManagerComponent->Equip(EquipmentSlotType, ItemInstance);
	}
	else if (const UPUBGItemFragment_Armor* ArmorFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Armor>())
	{
		EArmorType ArmorType = ArmorFragment->ArmorType;
		EEquipmentSlotType EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::ArmorTypeConvertToEquipmentSlotType(ArmorType);
		EquipmentManagerComponent->Equip(EquipmentSlotType, ItemInstance);
	}
	else if (const UPUBGItemFragment_Utility* UtilityFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Utility>())
	{
		EUtilityType UtilityType = UtilityFragment->UtilityType;
		EEquipmentSlotType EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::UtilityTypeConvertToEquipmentSlotType(UtilityType);
		EquipmentManagerComponent->Equip(EquipmentSlotType, ItemInstance);
	}
	
}

void UPUBGItemFunctionLibrary::BP_EquipEquipItem(int32 ItemTemplateID, AActor* Actor)
{
	NativeEquipEquipItem(ItemTemplateID, Actor);
}

void UPUBGItemFunctionLibrary::NativeUnequipEquipItem(EEquipmentSlotType EquipmentSlotType, AActor* Actor)
{
	UPUBGActiveEquipmentManagerComponent* EquipmentManagerComponent = Actor->GetComponentByClass<UPUBGActiveEquipmentManagerComponent>();

	if (EquipmentManagerComponent == nullptr)
	{
		return;
	}

	EquipmentManagerComponent->Unequip(EquipmentSlotType);
}

void UPUBGItemFunctionLibrary::BP_UnequipEquipItem(EEquipmentSlotType EquipmentSlotType, AActor* Actor)
{
	NativeUnequipEquipItem(EquipmentSlotType, Actor);
}

TArray<APUBGPickupableItemBase*> UPUBGItemFunctionLibrary::BP_AllItemSpawn(UObject* WorldContextObject)
{
	TArray<TSubclassOf<UPUBGItemTemplate>> OutItemTemplateClasses;
	UPUBGItemData::Get().GetAllItemTemplateClasses(OutItemTemplateClasses);

	TArray<APUBGPickupableItemBase*> pickupItem;
	for (auto itemTemplate : OutItemTemplateClasses)
	{
		int32 itemID = UPUBGItemData::Get().FindItemTemplateIDByClass(itemTemplate);
		pickupItem.Add(NativeCreatePickupableItemBase(WorldContextObject, itemID));
	}

	return pickupItem;
}

void UPUBGItemFunctionLibrary::GenerateItems(UObject* WorldContextObject)
{
	APUBGGameMode* GameMode = Cast<APUBGGameMode>(WorldContextObject->GetWorld()->GetAuthGameMode());
	GameMode->GenerateItems();
}
