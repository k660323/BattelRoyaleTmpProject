// Kakanu Portfolio Project


#include "GameMode/PUBGGameMode.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGItemData.h"
#include "Data/PUBGItemSpawnProbabilityData.h"
#include "Data/PUBGWorldItemSpawnData.h"
// ����
#include "Item/Fragments/PUBGItemFragment_Weapon_Gun.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Consumable.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "Item/Fragments/PUBGItemFragment_Ammo.h"
// ��Ÿ
#include "Item/PUBGItemTemplate.h"
#include "Item/PUBGItemInstance.h"
#include "Actors/PUBGPickupableItemBase.h"
#include "Interaction/PUBGPickupable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGameMode)

APUBGGameMode::APUBGGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void APUBGGameMode::GenerateItems()
{
	UPUBGItemSpawnProbabilityData* ItemSpawnProbabilityData = UPUBGAssetManager::GetAssetByName<UPUBGItemSpawnProbabilityData>(TEXT("ItemSpawnProbabilityData"));

	if(ItemSpawnProbabilityData == nullptr)
	{
		return;
	}
	
	const UPUBGWorldItemSpawnData* WorldItemSpawnData = ItemSpawnProbabilityData->GetWorldItemSpawnData();
	int32 SpawnPointCount = WorldItemSpawnData->GetItemSpawnPointTransformDataLength();

	// ���� ����Ʈ ����ŭ ����
	for (int32 i = 0; i < SpawnPointCount; i++)
	{
		// ���������� ������ ���� ������ ī���� �޾ƿ���
		int32 ItemSpawnCount = ItemSpawnProbabilityData->RandomItemSpawnCount();

		TArray<FTransform> Transforms = WorldItemSpawnData->GetRandomShuffleItemSpawnPointTransforms(i);

		for (int32 j = 0; j < ItemSpawnCount; j++)
		{
			EItemType ItemType;
			TPair<UEnum*, int32> MiddleClassificationItemType;
			int32 ItemNumber = 0;
			int32 TryIndex = 0;
			while (ItemNumber == 0 && TryIndex < 10)
			{
				TryIndex++;

				// ���� ������ ��з� ����
				ItemType = ItemSpawnProbabilityData->SelectRandomMajorCategoriesItemType();

				if (ItemType == EItemType::Count)
				{
					continue;
				}

				// ���� ������ �ߺз� ����
				MiddleClassificationItemType = ItemSpawnProbabilityData->SelectRandomMiddleClassificationItemType(ItemType);

				if (MiddleClassificationItemType.Key == nullptr)
				{
					continue;
				}

				// ���� ������ �Һз� ����
				ItemNumber = ItemSpawnProbabilityData->SelectRandomSubcategoriesItemID(MiddleClassificationItemType);

				if (ItemNumber == 0)
				{
					continue;
				}
			}

			if (ItemNumber == 0)
			{
				continue;
			}

			// ������ ����
			int32 ItemCount = 1;
			TObjectPtr<UPUBGItemInstance> ItemInstance = NativeCreateItemInstance(ItemNumber);
			if (ItemInstance->FindFragmentByClass<UPUBGItemFragment_Ammo>())
			{
				ItemCount = 30;
			}

			APUBGPickupableItemBase* PickupableItemBase = NativeSpawnPickupableItemByItemInstance(ItemInstance, ItemCount);
			PickupableItemBase->SetActorTransform(Transforms[j]);

			// �Ѿ� �߰� ����
			if (MiddleClassificationItemType.Key == StaticEnum<EWeaponType>())
			{
				if (MiddleClassificationItemType.Value == (int32)EWeaponType::Main)
				{
					if (const UPUBGItemFragment_Weapon_Gun* GunFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Weapon_Gun>())
					{
						// �߰� ���� ��������
						// ���� �ִ� �ε��� ���� ��������
						int32 PredictionRemainIndexLen = ItemSpawnCount - (j + 1);

						// �����ϰ� ���������� �Ѿ˼�
						int32 AmmoSpawnLen = FMath::RandRange(1, 3);

						// �� ª�� ���� �Ҵ�
						AmmoSpawnLen = FMath::Min(AmmoSpawnLen, PredictionRemainIndexLen);

						// ���ø����ε� �����Ҽ� �ְ�
						const UPUBGItemData& ItemData = UPUBGItemData::Get();
						int32 AmmoNumber = ItemData.FindItemTemplateIDByClass(GunFragment->AmmoItemTemplateClass);
						
						
						// �Ѿ� ����
						for (int32 k = 0; k < AmmoSpawnLen; k++)
						{
							APUBGPickupableItemBase* PickupableAmmoItemBase = NativeSpawnPickupableItemByID(AmmoNumber, 30);
							PickupableAmmoItemBase->SetActorTransform(Transforms[j + 1]);
							j++;
						}
					}
				
				}
			}
		}
	}
}

UPUBGItemInstance* APUBGGameMode::NativeCreateItemInstance(int32 ItemTemplateID)
{
	UPUBGItemInstance* ItemInstance = NewObject<UPUBGItemInstance>();
	ItemInstance->Init(ItemTemplateID);

	return ItemInstance;
}

APUBGPickupableItemBase* APUBGGameMode::NativeSpawnPickupableItemByID(int32 ItemTemplateID, int32 ItemCount)
{
	UPUBGItemInstance* ItemInstance = NativeCreateItemInstance(ItemTemplateID);

	FPUBGPickupInstance PickupInstance;
	PickupInstance.ItemInstance = ItemInstance;
	PickupInstance.ItemCount = ItemCount;

	TSubclassOf<APUBGPickupableItemBase> PickupableItemBaseClass = UPUBGAssetManager::GetSubclassByName<APUBGPickupableItemBase>("PickupableItemBaseClass");
	APUBGPickupableItemBase* PickupableItemBase = GetWorld()->SpawnActor<APUBGPickupableItemBase>(PickupableItemBaseClass);
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

APUBGPickupableItemBase* APUBGGameMode::NativeSpawnPickupableItemByItemInstance(UPUBGItemInstance* ItemInstance, int32 ItemCount)
{
	if (ItemInstance == nullptr)
	{
		return nullptr;
	}

	FPUBGPickupInstance PickupInstance;
	PickupInstance.ItemInstance = ItemInstance;
	PickupInstance.ItemCount = ItemCount;

	TSubclassOf<APUBGPickupableItemBase> PickupableItemBaseClass = UPUBGAssetManager::GetSubclassByName<APUBGPickupableItemBase>("PickupableItemBaseClass");
	APUBGPickupableItemBase* PickupableItemBase = GetWorld()->SpawnActor<APUBGPickupableItemBase>(PickupableItemBaseClass);
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
