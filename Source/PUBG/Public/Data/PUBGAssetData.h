// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PUBGAssetData.generated.h"

USTRUCT()
struct FAssetEntry
{
	GENERATED_BODY()

public:
	// 데이터 이름
	UPROPERTY(EditDefaultsOnly)
	FName AssetName;

	// 데이터 경로
	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath AssetPath;

	// 소속
	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AssetLables;
};

// FAssetSet 구조체는 FAssetEntry를 여럿 들고 있다.
USTRUCT()
struct FAssetSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAssetEntry> AssetEntries;
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAssetData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 저장시 캐싱 변수에 데이터를 삽입 
	// 
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

public:
	// 그룹명으로 경로를 들고온다.
	FSoftObjectPath GetAssetPathByName(const FName& AssetName);

	const FAssetSet& GetAssetSetByLabel(const FName& Label);


private:
	// FName(그룹)으로 FAssetSet 구조체를 저장한다.
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetSet> AssetGroupNameToSet;

	// ------------- 캐싱 데이터 ------------- 
	// 
	// FAssetEntry의 FName과 FSoftObjectPath를 TMap으로 들고 있는다
	UPROPERTY()
	TMap<FName, FSoftObjectPath> AssetNameToPath;

	// FName타입인 라벨로 FAssetSet을 TMap으로 들고 있는다.
	UPROPERTY()
	TMap<FName, FAssetSet> AssetLabelToSet;
};
