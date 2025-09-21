// Kakanu Portfolio Project


#include "GameMode/PUBGGameMode.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGItemData.h"
#include "Data/PUBGItemSpawnProbabilityData.h"
#include "Data/PUBGWorldItemSpawnData.h"
// 무기
#include "Item/Fragments/PUBGItemFragment_Weapon_Gun.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Consumable.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "Item/Fragments/PUBGItemFragment_Ammo.h"
// 기타
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

	// 스폰 포인트 수만큼 스폰
	for (int32 i = 0; i < SpawnPointCount; i++)
	{
		// 스폰지점에 스폰할 랜덤 아이템 카운터 받아오기
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

				// 랜덤 아이템 대분류 선택
				ItemType = ItemSpawnProbabilityData->SelectRandomMajorCategoriesItemType();

				if (ItemType == EItemType::Count)
				{
					continue;
				}

				// 랜덤 아이템 중분류 선택
				MiddleClassificationItemType = ItemSpawnProbabilityData->SelectRandomMiddleClassificationItemType(ItemType);

				if (MiddleClassificationItemType.Key == nullptr)
				{
					continue;
				}

				// 랜덤 아이템 소분류 선택
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

			// 아이템 스폰
			int32 ItemCount = 1;
			TObjectPtr<UPUBGItemInstance> ItemInstance = NativeCreateItemInstance(ItemNumber);
			if (ItemInstance->FindFragmentByClass<UPUBGItemFragment_Ammo>())
			{
				ItemCount = 30;
			}

			APUBGPickupableItemBase* PickupableItemBase = NativeSpawnPickupableItemByItemInstance(ItemInstance, ItemCount);
			PickupableItemBase->SetActorTransform(Transforms[j]);

			// 총알 추가 스폰
			if (MiddleClassificationItemType.Key == StaticEnum<EWeaponType>())
			{
				if (MiddleClassificationItemType.Value == (int32)EWeaponType::Main)
				{
					if (const UPUBGItemFragment_Weapon_Gun* GunFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Weapon_Gun>())
					{
						// 추가 스폰 가능한지
						// 남아 있는 인덱스 길이 가져오기
						int32 PredictionRemainIndexLen = ItemSpawnCount - (j + 1);

						// 랜덤하게 스폰가능한 총알수
						int32 AmmoSpawnLen = FMath::RandRange(1, 3);

						// 더 짧은 변수 할당
						AmmoSpawnLen = FMath::Min(AmmoSpawnLen, PredictionRemainIndexLen);

						// 템플릿으로도 스폰할수 있게
						const UPUBGItemData& ItemData = UPUBGItemData::Get();
						int32 AmmoNumber = ItemData.FindItemTemplateIDByClass(GunFragment->AmmoItemTemplateClass);
						
						
						// 총알 스폰
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
