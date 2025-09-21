// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Data/PUBGAssetData.h"
#include "PUBGAssetManager.generated.h"

// 비동기 에셋 로드 완료시 콜백 함수 호출하기 위한 델리게이트
DECLARE_DELEGATE_TwoParams(FAsyncLoadCompletedDelegate, const FName&/*AssetName or Label*/, UObject*/*LoadedAsset*/);
// 비동기 다중 에셋 로드 완료시 콜백 함수
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

	// 외부에서 클래스타입과 이름을 받아
	// 이름 기반으로 에셋을 찾아 해당 타입으로 반환하는 템플릿
	template<typename AssetType>
	static AssetType* GetAssetByPath(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static AssetType* GetAssetByName(const FName& AssetName);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclassByPath(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclassByName(const FName& AssetName, bool bKeepInMemory = true);

	// ----- 동기 로드 -----
	// 단일 로드
	static void LoadSyncByPath(const FSoftObjectPath& AssetPath);
	// 단일 로드
	static void LoadSyncByName(const FName& AssetName);
	// 다중 로드
	static void LoadSyncByLabel(const FName& Label);

	// ----- 비동기 로드 -----
	// 단일 로드
	static void LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	// 단일 로드
	static void LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompletedDelegate = FAsyncLoadCompletedDelegate());
	// 전체 비로드
	static void LoadAsyncByLabel(const FName& Label, FAsyncLoadLabelCompletedDelegate CompletedDelegate = FAsyncLoadLabelCompletedDelegate());

	// ----- 동기 비로드 -----
	// 단일 비로드
	static void ReleaseByPath(const FSoftObjectPath& AssetPath);
	// 단일 비로드
	static void ReleaseByName(const FName& AssetName);
	// 다중 비로드
	static void ReleaseByLabel(const FName& Label);
	// 전체 비로드
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
	// 모든 에셋을 관리하는 객체
	UPROPERTY()
	TObjectPtr<UPUBGAssetData> LoadedAssetData;

	// 캐싱 객체
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
		// 미리 로드하지 않으면 동기 로드 시작
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
