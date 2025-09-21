// Kakanu Portfolio Project


#include "Data/PUBGItemSpawnProbabilityData.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGItemData.h"
#include "Item/PUBGItemTemplate.h"
#include "Data/PUBGWorldItemSpawnData.h"
// 무기
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Consumable.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "Item/Fragments/PUBGItemFragment_Ammo.h"
// 기타
#if WITH_EDITORONLY_DATA
#include "Misc/DataValidation.h"
#include "UObject/ObjectSaveContext.h"
#endif // WITH_EDITORONLY_DATA

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemSpawnProbabilityData)

#if WITH_EDITORONLY_DATA
void UPUBGItemSpawnProbabilityData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	UPUBGItemData* LoadedItemData = UPUBGAssetManager::Get().GetAssetByPath(ItemData, false);
	if (LoadedItemData == nullptr)
	{
		return;
	}

	TArray<FPUBGItemModelProbability> TempArray;
	TSet<int32> ItemNumberSet;

	// 총 (미구현)
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EWeaponType>();
		int32 EnumNumber = (int32)EWeaponType::Main;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, GunWeaponItemModelData, TempArray, ItemNumberSet);
	}

	// 권총 (미구현)
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EWeaponType>();
		int32 EnumNumber = (int32)EWeaponType::Sub;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, SubGunWeaponItemModelData, TempArray, ItemNumberSet);
	}

	// 근접
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EWeaponType>();
		int32 EnumNumber = (int32)EWeaponType::ETC;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, MeleeWeaponItemModelData, TempArray, ItemNumberSet);
	}

	// 투척
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EWeaponType>();
		int32 EnumNumber = (int32)EWeaponType::Throw;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, ThrowWeaponItemModelData, TempArray, ItemNumberSet);
	}

	// 조준
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponAccItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EGunAccType>();
		int32 EnumNumber = (int32)EGunAccType::Sight;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, SightItemModelData, TempArray, ItemNumberSet);
	}

	// 총구
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponAccItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EGunAccType>();
		int32 EnumNumber = (int32)EGunAccType::Muzzle;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, MuzzleItemModelData, TempArray, ItemNumberSet);
	}

	// 손잡이
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponAccItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EGunAccType>();
		int32 EnumNumber = (int32)EGunAccType::Foregrip;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, ForegripItemModelData, TempArray, ItemNumberSet);
	}

	// 탄창
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponAccItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EGunAccType>();
		int32 EnumNumber = (int32)EGunAccType::Mag;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, MagItemModelData, TempArray, ItemNumberSet);
	}

	// 개머리판
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetWeaponAccItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EGunAccType>();
		int32 EnumNumber = (int32)EGunAccType::Buttstock;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, ButtstockItemModelData, TempArray, ItemNumberSet);
	}

	// 헬멧
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetArmorItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EArmorType>();
		int32 EnumNumber = (int32)EArmorType::Helmet;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, HelmetItemModelData, TempArray, ItemNumberSet);
	}

	
	// 조끼
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetArmorItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EArmorType>();
		int32 EnumNumber = (int32)EArmorType::Vest;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, VestItemModelData, TempArray, ItemNumberSet);
	}
	
	
	// 힐
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetConsumableItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EConsumptionType>();
		int32 EnumNumber = (int32)EConsumptionType::Heal;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, HealItemModelData, TempArray, ItemNumberSet);
	}


	// 마실꺼
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetConsumableItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EConsumptionType>();
		int32 EnumNumber = (int32)EConsumptionType::Drink;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, DrinkItemModelData, TempArray, ItemNumberSet);
	}


	// 상의
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetFashionItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EFashionType>();
		int32 EnumNumber = (int32)EFashionType::ClothTop;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, ClothTopItemModelData, TempArray, ItemNumberSet);
	}


	// 하의
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetFashionItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EFashionType>();
		int32 EnumNumber = (int32)EFashionType::ClothBottom;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, ClothBottomItemModelData, TempArray, ItemNumberSet);
	}


	// 한벌옷
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetFashionItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EFashionType>();
		int32 EnumNumber = (int32)EFashionType::Whole;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, WholeItemModelData, TempArray, ItemNumberSet);
	}

	// 신발
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetFashionItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EFashionType>();
		int32 EnumNumber = (int32)EFashionType::Shoes;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, ShoesItemModelData, TempArray, ItemNumberSet);
	}

	// 벨트
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetUtilityItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EUtilityType>();
		int32 EnumNumber = (int32)EUtilityType::Belt;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, BeltItemModelData, TempArray, ItemNumberSet);
	}

	// 가방
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetUtilityItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EUtilityType>();
		int32 EnumNumber = (int32)EUtilityType::Backpack;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, BackpackItemModelData, TempArray, ItemNumberSet);
	}


	// 총알
	{
		const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetAmmoItemTemplateClasses();
		UEnum* DestEnum = StaticEnum<EPayloadType>();
		int32 EnumNumber = (int32)EPayloadType::Ammo;
		CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, AmmoItemModelData, TempArray, ItemNumberSet);
	}

	// 기타 (미구현)
	{
		//const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses = LoadedItemData->GetEtcItemTemplateClasses();
		//UEnum* DestEnum = StaticEnum<EAmmoType>();
		//int32 EnumNumber = (int32)EAmmoType::Ammo;
		//CheckAndAdd(LoadedItemData, ItemTemplateClasses, DestEnum, EnumNumber, EtcItemModelData, TempArray, ItemNumberSet);
	}


	IsClear = false;
	IsCheckAndAddNewItem = false;
}

const FPUBGItemModelData* UPUBGItemSpawnProbabilityData::GetItemModelProbability(TSubclassOf<UPUBGItemFragment> FragmentClass, int32 ItemType) const
{
	if (FragmentClass->IsChildOf(UPUBGItemFragment_Weapon::StaticClass()))
	{
		switch ((EWeaponType)ItemType)
		{
		case EWeaponType::Main:
			return &GunWeaponItemModelData;
		case EWeaponType::Sub:
			return &SubGunWeaponItemModelData;
		case EWeaponType::ETC:
			return &MeleeWeaponItemModelData;
		case EWeaponType::Throw:
			return &ThrowWeaponItemModelData;
		}
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_GunAcc::StaticClass()))
	{
		switch ((EGunAccType)ItemType)
		{
		case EGunAccType::Sight:
			return &SightItemModelData;
		case EGunAccType::Muzzle:
			return &MuzzleItemModelData;
		case EGunAccType::Foregrip:
			return &ForegripItemModelData;
		case EGunAccType::Mag:
			return &MagItemModelData;
		case EGunAccType::Buttstock:
			return &ButtstockItemModelData;
		}
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Armor::StaticClass()))
	{
		switch ((EArmorType)ItemType)
		{
		case EArmorType::Helmet:
			return &HelmetItemModelData;
		case EArmorType::Vest:
			return &VestItemModelData;
		}
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Consumable::StaticClass()))
	{
		switch ((EConsumptionType)ItemType)
		{
		case EConsumptionType::Heal:
			return &HealItemModelData;
		case EConsumptionType::Drink:
			return &DrinkItemModelData;
		}
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Fashion::StaticClass()))
	{
		switch ((EFashionType)ItemType)
		{
		case EFashionType::ClothTop:
			return &ClothTopItemModelData;
		case EFashionType::ClothBottom:
			return &ClothBottomItemModelData;
		case EFashionType::Whole:
			return &WholeItemModelData;
		case EFashionType::Shoes:
			return &ShoesItemModelData;
		}
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Utility::StaticClass()))
	{
		switch ((EUtilityType)ItemType)
		{
		case EUtilityType::Backpack:
			return &BackpackItemModelData;
		case EUtilityType::Belt:
			return &BeltItemModelData;
		}
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Ammo::StaticClass()))
	{
		switch ((EPayloadType)ItemType)
		{
		case EPayloadType::Ammo:
			return &AmmoItemModelData;
		}
		
	}
	// 기타 미구현
	//
	//
	return nullptr;
}

int32 UPUBGItemSpawnProbabilityData::RandomItemSpawnCount() const
{
	TMap<FName, uint8*> RowMap = ItemSpawnCountProbability->GetRowMap();

	float CurrentPercent = 0.0f;
	float TotalPercent = 0.0f;
	for (const TPair<FName, uint8*>& Pair : RowMap)
	{
		FName RowName = Pair.Key;
		uint8* RawData = Pair.Value;

		FPUBGItemCountProbability* Row = reinterpret_cast<FPUBGItemCountProbability*>(RawData);
		if (Row)
		{
			TotalPercent += Row->Percent;
		}
	}

	float RandomValue = FMath::FRandRange(0.0f, TotalPercent);
	for (const TPair<FName, uint8*>& Pair : RowMap)
	{
		FName RowName = Pair.Key;
		uint8* RawData = Pair.Value;

		FPUBGItemCountProbability* Row = reinterpret_cast<FPUBGItemCountProbability*>(RawData);
		if (Row)
		{
			CurrentPercent += Row->Percent;
			if (RandomValue < CurrentPercent)
			{
				return Row->ItemCount;
			}
		}
	}

	return 0;
}

EItemType UPUBGItemSpawnProbabilityData::SelectRandomMajorCategoriesItemType() const
{
	TMap<FName, uint8*> RowMap = ItemTypeProbability->GetRowMap();

	float CurrentPercent = 0.0f;
	float TotalPercent = 0.0f;
	for (const TPair<FName, uint8*>& Pair : RowMap)
	{
		FName RowName = Pair.Key;
		uint8* RawData = Pair.Value;

		FPUBGItemTypeProbability* Row = reinterpret_cast<FPUBGItemTypeProbability*>(RawData);
		if (Row)
		{
			TotalPercent += Row->Percent;
		}
	}

	float RandomValue = FMath::FRandRange(0.0f, TotalPercent);
	for (const TPair<FName, uint8*>& Pair : RowMap)
	{
		FName RowName = Pair.Key;
		uint8* RawData = Pair.Value;

		FPUBGItemTypeProbability* Row = reinterpret_cast<FPUBGItemTypeProbability*>(RawData);
		if (Row)
		{
			if (Row->Percent == 0.0f)
			{
				continue;
			}

			CurrentPercent += Row->Percent;

			if (RandomValue <= CurrentPercent)
			{
				return Row->ItemType;
			}
		}
	}

	return EItemType::Count;
}

TPair<UEnum*, int32> UPUBGItemSpawnProbabilityData::SelectRandomMiddleClassificationItemType(EItemType ItemType) const
{
	TMap<FName, uint8*> RowMap;
	TPair<UEnum*, int32> ReturnPair;
	
	// 랜덤 아이템 중분류 선택
	switch (ItemType)
	{
	case EItemType::Weapon:
		RowMap = WeaponTypeProbability->GetRowMap();
		break;
	case EItemType::Armor:
		RowMap = ArmorTypeProbability->GetRowMap();
		break;
	case EItemType::Consumption:
		RowMap = ConsumptionTypeProbability->GetRowMap();
		break;
	case EItemType::Fashion:
		RowMap = FashionTypeProbability->GetRowMap();
		break;
	case EItemType::GunAcc:
		RowMap = GunAccTypeProbability->GetRowMap();
		break;
	case EItemType::Utility:
		RowMap = UtilityTypeProbability->GetRowMap();
		break;
	case EItemType::Payload:
		RowMap = PayloadTypeProbability->GetRowMap();
		break;
	case EItemType::ETC:
		RowMap = EtcTypeProbability->GetRowMap();
		break;
	}

	float CurrentPercent = 0.0f;
	float TotalPercent = 0.0f;
	for (const TPair<FName, uint8*>& Pair : RowMap)
	{
		FName RowName = Pair.Key;
		uint8* RawData = Pair.Value;

		FPUBGMiddleClassification* Row = reinterpret_cast<FPUBGMiddleClassification*>(RawData);
		if (Row)
		{
			TotalPercent += Row->GetPercent();
		}
	}

	float RandomValue = FMath::FRandRange(0.0f, TotalPercent);
	for (const TPair<FName, uint8*>& Pair : RowMap)
	{
		FName RowName = Pair.Key;
		uint8* RawData = Pair.Value;

		FPUBGMiddleClassification* Row = reinterpret_cast<FPUBGMiddleClassification*>(RawData);
		if (Row)
		{
			if (Row->GetPercent() == 0.0f)
			{
				continue;
			}

			CurrentPercent += Row->GetPercent();

			if (RandomValue <= CurrentPercent)
			{
				ReturnPair = Row->GetEnumInfoPair();
				break;
			}
		}
	}

	return ReturnPair;
}

int32 UPUBGItemSpawnProbabilityData::SelectRandomSubcategoriesItemID(TPair<UEnum*, int32>& EnumInfoPair) const
{
	int32 ItemNumber = 0;

	FPUBGItemModelData ItemModelData;

	if (EnumInfoPair.Key == StaticEnum<EWeaponType>())
	{
		switch ((EWeaponType)EnumInfoPair.Value)
		{
		case EWeaponType::Main:
			ItemModelData = GunWeaponItemModelData;
			break;
		case EWeaponType::ETC:
			ItemModelData = MeleeWeaponItemModelData;
			break;
		case EWeaponType::Throw:
			ItemModelData = ThrowWeaponItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EGunAccType>())
	{
		switch ((EGunAccType)EnumInfoPair.Value)
		{
		case EGunAccType::Sight:
			ItemModelData = SightItemModelData;
			break;
		case EGunAccType::Muzzle:
			ItemModelData = MuzzleItemModelData;
			break;
		case EGunAccType::Foregrip:
			ItemModelData = ForegripItemModelData;
			break;
		case EGunAccType::Mag:
			ItemModelData = MagItemModelData;
			break;
		case EGunAccType::Buttstock:
			ItemModelData = ButtstockItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EArmorType>())
	{
		switch ((EArmorType)EnumInfoPair.Value)
		{
		case EArmorType::Helmet:
			ItemModelData = HelmetItemModelData;
			break;
		case EArmorType::Vest:
			ItemModelData = VestItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EConsumptionType>())
	{
		switch ((EConsumptionType)EnumInfoPair.Value)
		{
		case EConsumptionType::Heal:
			ItemModelData = HealItemModelData;
			break;
		case EConsumptionType::Drink:
			ItemModelData = DrinkItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EFashionType>())
	{
		switch ((EFashionType)EnumInfoPair.Value)
		{
		case EFashionType::ClothTop:
			ItemModelData = ClothTopItemModelData;
			break;
		case EFashionType::ClothBottom:
			ItemModelData = ClothBottomItemModelData;
			break;
		case EFashionType::Whole:
			ItemModelData = WholeItemModelData;
			break;
		case EFashionType::Shoes:
			ItemModelData = ShoesItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EUtilityType>())
	{
		switch ((EUtilityType)EnumInfoPair.Value)
		{
		case EUtilityType::Backpack:
			ItemModelData = BackpackItemModelData;
			break;
		case EUtilityType::Belt:
			ItemModelData = BeltItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EPayloadType>())
	{
		switch ((EPayloadType)EnumInfoPair.Value)
		{
		case EPayloadType::Ammo:
			ItemModelData = AmmoItemModelData;
			break;
		}
	}
	else if (EnumInfoPair.Key == StaticEnum<EEtcType>())
	{
		switch ((EEtcType)EnumInfoPair.Value)
		{
		case EEtcType::None:
			ItemModelData = EtcItemModelData;
			break;
		}
	}

	float CurrentPercent = 0.0f;
	float TotalPercent = ItemModelData.TotalPercent;
	float RandomValue = FMath::FRandRange(0.0f, TotalPercent);

	for (const FPUBGItemModelProbability& ItemModelProbability : ItemModelData.ItemModelProbabilitySet)
	{
		if (ItemModelProbability.Percent == 0.0f)
		{
			continue;
		}

		CurrentPercent += ItemModelProbability.Percent;

		if (RandomValue <= CurrentPercent)
		{
			ItemNumber = ItemModelProbability.ItemNumber;
			break;
		}
	}

	return ItemNumber;
}

UPUBGWorldItemSpawnData* UPUBGItemSpawnProbabilityData::GetWorldItemSpawnData() const
{
	return WorldItemSpawnData;
}

const UDataTable* UPUBGItemSpawnProbabilityData::GetItemSpawnCountProbability() const
{
	return ItemSpawnCountProbability;
}

const UDataTable* UPUBGItemSpawnProbabilityData::GetMajorCategoriesItemTypeDataTable() const
{
	return ItemTypeProbability;
}

const UDataTable* UPUBGItemSpawnProbabilityData::GetMiddleClassificationItemTypeDataTable(TSubclassOf<UPUBGItemFragment> FragmentClass) const
{
	if (FragmentClass->IsChildOf(UPUBGItemFragment_Weapon::StaticClass()))
	{
		return WeaponTypeProbability;
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_GunAcc::StaticClass()))
	{
		return GunAccTypeProbability;
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Armor::StaticClass()))
	{
		return ArmorTypeProbability;
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Consumable::StaticClass()))
	{
		return ConsumptionTypeProbability;
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Fashion::StaticClass()))
	{
		return FashionTypeProbability;
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Utility::StaticClass()))
	{
		return UtilityTypeProbability;
	}
	else if (FragmentClass->IsChildOf(UPUBGItemFragment_Ammo::StaticClass()))
	{
		return PayloadTypeProbability;
	}

	// 기타 미구현
	//
	//

	return nullptr;
}


void UPUBGItemSpawnProbabilityData::ItemDuplicateCheck(UPUBGItemData* LoadedItemData, UEnum* DestEnum, int32& EnumNumber, FPUBGItemModelData& DestItemModelData, OUT TArray<FPUBGItemModelProbability>& TempArray, OUT TSet<int32>& TempItemIDSet)
{
	// 기존 아이템 타입 유효성 체크, 중복 체크하여 새배열에 담기
	for (FPUBGItemModelProbability& ItemModleProbability : DestItemModelData.ItemModelProbabilitySet)
	{
		if (ItemModleProbability.ItemNumber <= 0)
		{
			continue;
		}

		const UPUBGItemTemplate& ItemTemplate = LoadedItemData->FindItemTemplateByID(ItemModleProbability.ItemNumber);
		
		if (const UPUBGItemFragment_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Weapon>())
		{
			if (DestEnum == StaticEnum<EWeaponType>() && WeaponFragment->WeaponType == (EWeaponType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		else if (const UPUBGItemFragment_GunAcc* GunAccFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_GunAcc>())
		{
			if (DestEnum == StaticEnum<EGunAccType>() && GunAccFragment->GunAccType == (EGunAccType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		else if (const UPUBGItemFragment_Armor* ArmorFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Armor>())
		{
			if (DestEnum == StaticEnum<EArmorType>() && ArmorFragment->ArmorType == (EArmorType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		else if (const UPUBGItemFragment_Consumable* ConsumableFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Consumable>())
		{
			if (DestEnum == StaticEnum<EConsumptionType>() && ConsumableFragment->ConsumptionType == (EConsumptionType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		else if (const UPUBGItemFragment_Fashion* FashionFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Fashion>())
		{
			if (DestEnum == StaticEnum<EFashionType>() && FashionFragment->FashionType == (EFashionType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		else if (const UPUBGItemFragment_Utility* UtilityFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Utility>())
		{
			if (DestEnum == StaticEnum<EUtilityType>() && UtilityFragment->UtilityType == (EUtilityType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		else if (const UPUBGItemFragment_Ammo* AmmoFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Ammo>())
		{
			if (DestEnum == StaticEnum<EPayloadType>() && AmmoFragment->PayloadType == (EPayloadType)EnumNumber)
			{
				TempArray.Add(ItemModleProbability);
				TempItemIDSet.Add(ItemModleProbability.ItemNumber);
			}
		}
		// 기타 미구현
		//
		//
	}
}

void UPUBGItemSpawnProbabilityData::NewItemAdd(UPUBGItemData* LoadedItemData, const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses, int32& EnumNumber, FPUBGItemModelData& DestItemModelData, TArray<FPUBGItemModelProbability>& TempArray, TSet<int32>& TempItemIDSet)
{
	for (const TSubclassOf<UPUBGItemTemplate>& ItemTemplateClass : ItemTemplateClasses)
	{
		int32 ItemNumber = LoadedItemData->FindItemTemplateIDByClass(ItemTemplateClass);
		if (TempItemIDSet.Contains(ItemNumber))
		{
			continue;
		}

		const UPUBGItemTemplate* ItemTemplate = Cast<UPUBGItemTemplate>(ItemTemplateClass->GetDefaultObject());
		if (ItemTemplate == nullptr)
		{
			continue;
		}

		if (const UPUBGItemFragment_Weapon* WeaponFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Weapon>())
		{
			if (WeaponFragment->WeaponType == (EWeaponType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		else if (const UPUBGItemFragment_GunAcc* GunAccFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_GunAcc>())
		{
			if (GunAccFragment->GunAccType == (EGunAccType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		else if (const UPUBGItemFragment_Armor* ArmorFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Armor>())
		{
			if (ArmorFragment->ArmorType == (EArmorType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		else if (const UPUBGItemFragment_Consumable* ConsumableFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Consumable>())
		{
			if (ConsumableFragment->ConsumptionType == (EConsumptionType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		else if (const UPUBGItemFragment_Fashion* FashionFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Fashion>())
		{
			if (FashionFragment->FashionType == (EFashionType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		else if (const UPUBGItemFragment_Utility* UtilityFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Utility>())
		{
			if (UtilityFragment->UtilityType == (EUtilityType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		else if (const UPUBGItemFragment_Ammo* AmmoFragment = ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Ammo>())
		{
			if (AmmoFragment->PayloadType == (EPayloadType)EnumNumber)
			{
				FPUBGItemModelProbability ItemModleProbability;
				ItemModleProbability.DisplayName = ItemTemplate->DisplayName;
				ItemModleProbability.IconTexture = ItemTemplate->IconTexture;
				ItemModleProbability.ItemNumber = ItemNumber;
				TempArray.Add(ItemModleProbability);
			}
		}
		// 기타 미구현
		//
		//
	}

	DestItemModelData.ItemModelProbabilitySet = TempArray;
}

void UPUBGItemSpawnProbabilityData::CheckAndAdd(UPUBGItemData* LoadedItemData, const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses, UEnum* DestEnum, int32& EnumNumber, FPUBGItemModelData& DestItemModelData, TArray<FPUBGItemModelProbability>& TempArray, TSet<int32>& TempItemIDSet)
{
	if (IsClear)
	{
		DestItemModelData.ItemModelProbabilitySet.Empty();
		DestItemModelData.TotalPercent = 0.0f;
	}

	ItemDuplicateCheck(LoadedItemData, DestEnum, EnumNumber, DestItemModelData, TempArray, TempItemIDSet);
	
	if (IsCheckAndAddNewItem)
	{
		NewItemAdd(LoadedItemData, ItemTemplateClasses, EnumNumber, DestItemModelData, TempArray, TempItemIDSet);
	}
	
	CalculationTotalPercent(DestItemModelData);


	TempArray.Empty();
	TempItemIDSet.Empty();
	DestItemModelData.ItemModelProbabilitySet.Sort([](const FPUBGItemModelProbability& ItemA, const FPUBGItemModelProbability& ItemB)
		{
			return ItemA.ItemNumber < ItemB.ItemNumber;
		});
}

void UPUBGItemSpawnProbabilityData::CalculationTotalPercent(FPUBGItemModelData& DestItemModelData)
{
	float TotalPercent = 0.0f;

	for (FPUBGItemModelProbability& ItemModelProbability : DestItemModelData.ItemModelProbabilitySet)
	{
		TotalPercent += ItemModelProbability.Percent;
	}

	DestItemModelData.TotalPercent = TotalPercent;
}

#endif // WITH_EDITORONLY_DATA

