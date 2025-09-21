// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Data/PUBGAssetData.h"
#include "PUBGAssetManager.generated.h"

// �񵿱� ���� �ε� �Ϸ�� �ݹ� �Լ� ȣ���ϱ� ���� ��������Ʈ
DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName&/*AssetName or Label*/, UObject*/*LoadedAsset*/);
// �񵿱� ���� ���� �ε� �Ϸ�� �ݹ� �Լ�
DECLARE_DELEGATE_ThreeParams(FAsyncLoadLabelCompletedDelegate, const FName& /*LabelName*/, const int /*loadedAssetCnt*/, const int /*TotalAssetCnt*/);

class UPUBGItemData;
/**
 *
 */
UCLASS(Config = Game)
class PUBG_API UPUBGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UPUBGAssetManager();

	//~UAssetManager interface
	virtual void StartInitialLoading() override;
#if WITH_EDITOR
	virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~End of UAssetManager interface

	FORCEINLINE static UPUBGAssetManager& Get()
	{
		if (UPUBGAssetManager* Singleton = Cast<UPUBGAssetManager>(GEngine->AssetManager))
		{
			return *Singleton;
		}

		UE_LOG(LogTemp, Fatal, TEXT("Can't find UPUBGAssetManager"));

		return *NewObject<UPUBGAssetManager>();
	};

public:
	static void Initialize();

	// �ܺο��� Ŭ����Ÿ�԰� �̸��� �޾�
	// �̸� ������� ������ ã�� �ش� Ÿ������ ��ȯ�ϴ� ���ø�
	template<typename AssetType>
	static AssetType* GetAssetByPath(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static AssetType* GetAssetByName(const FName& AssetName);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclassByPath(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclassByName(const FName& AssetName, bool bKeepInMemory = true);

	// ----- ���� �ε� -----
	// ���� �ε�
	static void LoadSyncByPath(const FSoftObjectPath& AssetPath);
	// ���� �ε�
	static void LoadSyncByName(const FName& AssetName);
	// ���� �ε�
	static void LoadSyncByLabel(const FName& Label);

	// ----- �񵿱� �ε� -----
	// ���� �ε�
	static void LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	// ���� �ε�
	static void LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	// ��ü ��ε�
	static void LoadAsyncByLabel(const FName& Label, FAsyncLoadLabelCompletedDelegate CompletedDelegate = FAsyncLoadLabelCompletedDelegate());

	// ----- ���� ��ε� -----
	// ���� ��ε�
	static void ReleaseByPath(const FSoftObjectPath& AssetPath);
	// ���� ��ε�
	static void ReleaseByName(const FName& AssetName);
	// ���� ��ε�
	static void ReleaseByLabel(const FName& Label);
	// ��ü ��ε�
	static void ReleaseAll();

	
public:
	// Primary Asset Start
	const UPUBGItemData& GetItemData();
	// Primary Asset End

protected:
	// Primary Asset Start
	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);
	// Primary Asset End

private:
	void LoadPreloadAssets();

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);
	void AddLoadedAsset(const FName& AssetName, const UObject* Asset);

protected:
	// Primary Asset Start
	UPROPERTY(Config)
	TSoftObjectPtr<UPUBGItemData> ItemDataPath;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;
	// Primary Asset End

private:
	// ��� ������ �����ϴ� ��ü
	UPROPERTY()
	TObjectPtr<UPUBGAssetData> LoadedAssetData;

	// ĳ�� ��ü
	UPROPERTY()
	TMap<FName, TObjectPtr<const UObject>> NameToLoadedAsset;

private:
	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;
};

template<typename AssetType>
AssetType* UPUBGAssetManager::GetAssetByPath(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();

		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename AssetType>
AssetType* UPUBGAssetManager::GetAssetByName(const FName& AssetName)
{
	UPUBGAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	if (AssetPath.IsValid())
	{
		LoadedAsset = Cast<AssetType>(AssetPath.ResolveObject());
		// �̸� �ε����� ������ ���� �ε� ����
		if (LoadedAsset == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempted sync loading because asset hadn't loaded yet [%s]."), *AssetPath.ToString());
			LoadedAsset = Cast<AssetType>(AssetPath.TryLoad());
		}
	}
	return LoadedAsset;
}

template<typename AssetType>
inline TSubclassOf<AssetType> UPUBGAssetManager::GetSubclassByPath(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}

template<typename AssetType>
inline TSubclassOf<AssetType> UPUBGAssetManager::GetSubclassByName(const FName& AssetName, bool bKeepInMemory)
{
	UPUBGAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);
	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);

	FString AssetPathString = AssetPath.GetAssetPathString();
	AssetPathString.Append(TEXT("_C"));

	FSoftClassPath ClassPath(AssetPathString);
	TSoftClassPtr<AssetType> ClassPtr(ClassPath);
	return GetSubclassByPath<AssetType>(ClassPtr, bKeepInMemory);
}
