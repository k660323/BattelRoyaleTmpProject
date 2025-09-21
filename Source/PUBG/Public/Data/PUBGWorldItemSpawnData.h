// Kakanu Portfolio Project

#pragma once

#include "Engine/DataAsset.h"
#include "PUBGWorldItemSpawnData.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnPointData
{
	GENERATED_BODY()

public:
	FItemSpawnPointData() {};
	FItemSpawnPointData(TArray<FTransform> TransformArray) : ItemSpawnPointTransforms(TransformArray) {};

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTransform> ItemSpawnPointTransforms;
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGWorldItemSpawnData : public UDataAsset
{
	GENERATED_BODY()
	
public:
#if WITH_EDITORONLY_DATA
	// 저장시 월드에 있는 액터 포지션 저장
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif //WITH_EDITORONLY_DATA

	int32 GetItemSpawnPointTransformDataLength() const;
	const FItemSpawnPointData* GetItemSpawnPointTransformData(int32 index) const;
	const TArray<FItemSpawnPointData>& GetItemSpawnPointTransformDatas() const;

	const TArray<FTransform>* GetItemSpawnPointTransforms(int32 index) const;
	TArray<FTransform> GetRandomShuffleItemSpawnPointTransforms(int32 index) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FItemSpawnPointData> ItemSpawnPointTransformData;
};
