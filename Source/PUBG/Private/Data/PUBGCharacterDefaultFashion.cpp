// Kakanu Portfolio Project


#include "Data/PUBGCharacterDefaultFashion.h"
#include "Data/PUBGItemData.h"
#include "Item/PUBGItemTemplate.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "System/PUBGAssetManager.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGCharacterDefaultFashion)

UPUBGCharacterDefaultFashion::UPUBGCharacterDefaultFashion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultMaskTexture(nullptr)
	, DefaultFashionDatas()
	, DefaultFashionDataMap()
{
	DefaultFashionDatas.Empty();

	DefaultFashionDatas.SetNum((int32)EFashionType::Count);

	for (int32 i = 0; i < DefaultFashionDatas.Num(); i++)
	{
		DefaultFashionDatas[i].FashionType = (EFashionType)i;
		DefaultFashionDatas[i].ItemTempalteID = 0;
	}
}

#if WITH_EDITORONLY_DATA
void UPUBGCharacterDefaultFashion::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	for (int32 i = 0; i < DefaultFashionDatas.Num(); i++)
	{
		if (DefaultFashionDatas[i].IsValid())
		{
			DefaultFashionDataMap.Add(DefaultFashionDatas[i].FashionType, DefaultFashionDatas[i]);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

UTexture* UPUBGCharacterDefaultFashion::GetDefaultMaskTexture() const
{
	return DefaultMaskTexture;
}

void UPUBGCharacterDefaultFashion::GetFashionMesh(EFashionType InFashionType, USkeletalMesh*& OutSkeletalMesh, UTexture*& OutTexture) const
{
	FPUBGCharacterDefaultFashionData FashionData = DefaultFashionDatas[(int32)InFashionType];

	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(FashionData.ItemTempalteID);
	const UPUBGItemFragment_Fashion* FashionFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Fashion>();

	if (FashionFragment == nullptr)
	{
		return;
	}

	OutSkeletalMesh = (UPUBGAssetManager::Get().GetAssetByPath(FashionFragment->FashionMesh));
	OutTexture = (UPUBGAssetManager::Get().GetAssetByPath(FashionFragment->MaskTexture));
}

FPUBGCharacterDefaultFashionData UPUBGCharacterDefaultFashion::GetFashionData(EFashionType InFashionType)
{
	return DefaultFashionDataMap[InFashionType];
}
