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
	// ������ �̸�
	UPROPERTY(EditDefaultsOnly)
	FName AssetName;

	// ������ ���
	UPROPERTY(EditDefaultsOnly)
	FSoftObjectPath AssetPath;

	// �Ҽ�
	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AssetLables;
};

// FAssetSet ����ü�� FAssetEntry�� ���� ��� �ִ�.
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
	// ����� ĳ�� ������ �����͸� ���� 
	// 
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;

public:
	// �׷������ ��θ� ���´�.
	FSoftObjectPath GetAssetPathByName(const FName& AssetName);

	const FAssetSet& GetAssetSetByLabel(const FName& Label);


private:
	// FName(�׷�)���� FAssetSet ����ü�� �����Ѵ�.
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FAssetSet> AssetGroupNameToSet;

	// ------------- ĳ�� ������ ------------- 
	// 
	// FAssetEntry�� FName�� FSoftObjectPath�� TMap���� ��� �ִ´�
	UPROPERTY()
	TMap<FName, FSoftObjectPath> AssetNameToPath;

	// FNameŸ���� �󺧷� FAssetSet�� TMap���� ��� �ִ´�.
	UPROPERTY()
	TMap<FName, FAssetSet> AssetLabelToSet;
};
