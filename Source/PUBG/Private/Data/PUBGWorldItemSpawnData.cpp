// Kakanu Portfolio Project


#include "Data/PUBGWorldItemSpawnData.h"
#include "EditorActors/PUBGItemSpawnPoint.h"
#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#if WITH_EDITORONLY_DATA
#include "UObject/ObjectSaveContext.h"
#endif // WITH_EDITORONLY_DATA

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGWorldItemSpawnData)

#if WITH_EDITORONLY_DATA
void UPUBGWorldItemSpawnData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	ItemSpawnPointTransformData.Empty();

	UWorld* CurrentWorld  = GWorld->GetWorld();

	for (TActorIterator<APUBGItemSpawnPoint> It(CurrentWorld); It; ++It)
	{
		APUBGItemSpawnPoint* ItemGroup = *It;
		if (ItemGroup)
		{
			TArray<FTransform> Array = ItemGroup->GetItemSpawnWorldTransform();
			FItemSpawnPointData ItemSpawnPointData(Array);
			ItemSpawnPointTransformData.Add(ItemSpawnPointData);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

int32 UPUBGWorldItemSpawnData::GetItemSpawnPointTransformDataLength() const
{
	return ItemSpawnPointTransformData.Num();
}

const FItemSpawnPointData* UPUBGWorldItemSpawnData::GetItemSpawnPointTransformData(int32 index) const
{
	if (index < 0 || ItemSpawnPointTransformData.Num() >= index)
	{
		return nullptr;
	}

	return &ItemSpawnPointTransformData[index];
}

const TArray<FItemSpawnPointData>& UPUBGWorldItemSpawnData::GetItemSpawnPointTransformDatas() const
{
	return ItemSpawnPointTransformData;
}

const TArray<FTransform>* UPUBGWorldItemSpawnData::GetItemSpawnPointTransforms(int32 index) const
{
	if (index < 0 || ItemSpawnPointTransformData.Num() >= index)
	{
		return nullptr;
	}

	return &ItemSpawnPointTransformData[index].ItemSpawnPointTransforms;
}

TArray<FTransform> UPUBGWorldItemSpawnData::GetRandomShuffleItemSpawnPointTransforms(int32 index) const
{
	if (index < 0 || ItemSpawnPointTransformData.Num() <= index)
	{
		return TArray<FTransform>();
	}

	TArray<FTransform> temp = ItemSpawnPointTransformData[index].ItemSpawnPointTransforms;
	Algo::RandomShuffle(temp);

	return temp;
}