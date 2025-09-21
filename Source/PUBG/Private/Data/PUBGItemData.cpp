// Kakanu Portfolio Project


#include "Data/PUBGItemData.h"
#include "System/PUBGAssetManager.h"
#include "Item/PUBGItemTemplate.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Consumable.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "Item/Fragments/PUBGItemFragment_Ammo.h"

#if WITH_EDITORONLY_DATA
#include "UObject/ObjectSaveContext.h"
#include "Misc/DataValidation.h"
#endif // WITH_EDITORONLY_DATA




#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemData)

const UPUBGItemData& UPUBGItemData::Get()
{
	return UPUBGAssetManager::Get().GetItemData();
}

#if WITH_EDITORONLY_DATA
void UPUBGItemData::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	// 오름 차순 정렬
	ItemTemplateIDToClass.KeySort([](const int32 A, const int32 B) {
		return A < B;
		});

	ItemTemplateClassToID.Empty();
	WeaponItemTemplateClasses.Empty();
	ArmorItemTemplateClasses.Empty();
	ConsumableItemTemplateClasses.Empty();
	FashionItemTemplateClasses.Empty();
	WeaponAccItemTemplateClasses.Empty();
	UtilityItemTemplateClasses.Empty();
	AmmoItemTemplateClasses.Empty();
	EtcItemTemplateClasses.Empty();

	for (const auto& Pair : ItemTemplateIDToClass)
	{
		ItemTemplateClassToID.Emplace(Pair.Value, Pair.Key);

		const UPUBGItemTemplate* ItemTemplate = Pair.Value.GetDefaultObject();
		if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Weapon>())
		{
			WeaponItemTemplateClasses.Add(Pair.Value);
		}
		else if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Armor>())
		{
			ArmorItemTemplateClasses.Add(Pair.Value);
		}
		else if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Consumable>())
		{
			ConsumableItemTemplateClasses.Add(Pair.Value);
		}
		else if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Fashion>())
		{
			FashionItemTemplateClasses.Add(Pair.Value);
		}
		else if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_GunAcc>())
		{
			WeaponAccItemTemplateClasses.Add(Pair.Value);
		}
		else if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Utility>())
		{
			UtilityItemTemplateClasses.Add(Pair.Value);
		}
		else if (ItemTemplate->FindFragmentByClass<UPUBGItemFragment_Ammo>())
		{
			AmmoItemTemplateClasses.Add(Pair.Value);
		}
		// 기타 (미구현)

	}

}
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
EDataValidationResult UPUBGItemData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	// 아이디 중복 체크용
	TSet<int32> ItemTemplateIDSet;
	// 클래스 중복 체크용
	TSet<TSubclassOf<UPUBGItemTemplate>> ItemTemplateClassSet;

	// 아이템 맵 순회
	for (const auto& Pair : ItemTemplateIDToClass)
	{
		// ID Check.
		const int32 ItemTemplateID = Pair.Key;

		// 유요한 아이디 체크
		if (ItemTemplateID <= 0)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid ID : [ID : %d], ID is over than 0\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		// 중복 체크
		if (ItemTemplateIDSet.Contains(ItemTemplateID))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated ID : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		ItemTemplateIDSet.Add(ItemTemplateID);

		// Class Check
		const TSubclassOf<UPUBGItemTemplate> ItemTemplateClass = Pair.Value;

		if (ItemTemplateClass == nullptr)
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Class : [ID : %d], Please Setting or Remove Element\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		if (ItemTemplateClassSet.Contains(ItemTemplateClass))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Duplicated Class : [ID : %d]\n"), ItemTemplateID)));
			Result = EDataValidationResult::Invalid;
		}

		ItemTemplateClassSet.Add(ItemTemplateClass);
	}

	return Result;
}
#endif // WITH_EDITOR

const UPUBGItemTemplate& UPUBGItemData::FindItemTemplateByID(int32 ItemTemplateID) const
{
	const TSubclassOf<UPUBGItemTemplate>* ItemTemplateClass = ItemTemplateIDToClass.Find(ItemTemplateID);
	ensureAlwaysMsgf(ItemTemplateClass, TEXT("Can't find ItemTemplateClass From ID [%d]"), ItemTemplateID);

	return *(ItemTemplateClass->GetDefaultObject());
}

int32 UPUBGItemData::FindItemTemplateIDByClass(TSubclassOf<UPUBGItemTemplate> ItemTemplateClass) const
{
	const int32* ItemTemplateID = ItemTemplateClassToID.Find(ItemTemplateClass);
	ensureAlwaysMsgf(ItemTemplateID, TEXT("Can't find ItemTemplateID from Class"));
	return *ItemTemplateID;
}

void UPUBGItemData::GetAllItemTemplateClasses(TArray<TSubclassOf<UPUBGItemTemplate>>& OutItemTemplateClasses) const
{
	OutItemTemplateClasses.Reset();
	OutItemTemplateClasses.Reserve(ItemTemplateIDToClass.Num());

	for (auto& Pair : ItemTemplateIDToClass)
	{
		OutItemTemplateClasses.Add(Pair.Value);
	}
}
