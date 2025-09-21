// Fill out your copyright notice in the Description page of Project Settings.


#include "System/PUBGAssetManager.h"
#include "Data/PUBGAssetData.h"
#include "Data/PUBGItemData.h"

UPUBGAssetManager::UPUBGAssetManager() : Super()
{
}

void UPUBGAssetManager::StartInitialLoading()
{
	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();

}

void UPUBGAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

}

void UPUBGAssetManager::Initialize()
{
	// TODO Primary Asset Load
	Get().LoadPreloadAssets();
	Get().GetItemData();
}

void UPUBGAssetManager::LoadSyncByPath(const FSoftObjectPath& AssetPath)
{
	// 에셋 경로가 유효한지
	if (AssetPath.IsValid())
	{
		UObject* LoadedAsset = AssetPath.ResolveObject();
		if (LoadedAsset == nullptr)
		{
			// 첫 초기화 시
			if (UAssetManager::IsInitialized())
			{
				// 에셋 로드할때 까지 대기
				LoadedAsset = UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
			}
			// 다시 시도
			else
			{
				LoadedAsset = AssetPath.TryLoad();
			}
		}

		// 에셋 로드 완료시 해당 에셋 추가
		if (LoadedAsset)
		{
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
		}
		// 에러 발생
		else
		{
			UE_LOG(LogTemp, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
		}
	}
}

void UPUBGAssetManager::LoadSyncByName(const FName& AssetName)
{
	UPUBGAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	LoadSyncByPath(AssetPath);
}

void UPUBGAssetManager::LoadSyncByLabel(const FName& Label)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	UPUBGAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	TArray<FSoftObjectPath> AssetPaths;

	const FAssetSet& AssetSet = AssetData->GetAssetSetByLabel(Label);
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		// 단일 로드
		LoadSyncByPath(AssetPath);
		if (AssetPath.IsValid())
		{
			AssetPaths.Emplace(AssetPath);
		}
	}

	// 다중 동기 로드 (앞에 로드한 에셋을 다시 로드하여 캐싱)
	GetStreamableManager().RequestSyncLoad(AssetPaths);

	// 캐싱하기 위해 다시 순회
	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		if (AssetPath.IsValid())
		{
			if (UObject* LoadedAsset = AssetPath.ResolveObject())
			{
				Get().AddLoadedAsset(AssetEntry.AssetName, LoadedAsset);
			}
			else
			{
				UE_LOG(LogTemp, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
			}
		}
	}
}

void UPUBGAssetManager::LoadAsyncByPath(const FSoftObjectPath& AssetPath, FAsyncLoadCompletedDelegate CompletedDelegate)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	if (AssetPath.IsValid())
	{
		if (UObject* LoadedAsset = AssetPath.ResolveObject())
		{
			Get().AddLoadedAsset(AssetPath.GetAssetFName(), LoadedAsset);
		}
		else
		{
			TArray<FSoftObjectPath> AssetPaths;
			AssetPaths.Add(AssetPath);

			TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(AssetPaths);

			Handle->BindCompleteDelegate(FStreamableDelegate::CreateLambda([AssetName = AssetPath.GetAssetFName(), AssetPath, CompleteDelegate = MoveTemp(CompletedDelegate)]()
				{
					UObject* LoadedAsset = AssetPath.ResolveObject();
					Get().AddLoadedAsset(AssetName, LoadedAsset);
					if (CompleteDelegate.IsBound())
						CompleteDelegate.Execute(AssetName, LoadedAsset);
				}));
		}
	}
}

void UPUBGAssetManager::LoadAsyncByName(const FName& AssetName, FAsyncLoadCompletedDelegate CompletedDelegate)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	UPUBGAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	const FSoftObjectPath& AssetPath = AssetData->GetAssetPathByName(AssetName);
	LoadAsyncByPath(AssetPath, CompletedDelegate);
}

void UPUBGAssetManager::LoadAsyncByLabel(const FName& Label, FAsyncLoadLabelCompletedDelegate CompletedDelegate)
{
	if (UAssetManager::IsInitialized() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AssetManager must be initialized"));
		return;
	}

	UPUBGAssetData* AssetData = Get().LoadedAssetData;
	check(AssetData);

	const FAssetSet& AssetSet = AssetData->GetAssetSetByLabel(Label);
	int cnt = 0, maxCnt = AssetSet.AssetEntries.Num();

	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
		// 단일 비로드 (콜백함수로 완료시 람다 함수로 카운트 증가 및 캐싱 모든 에셋을 로드하면 완료 람다 호출)
		LoadAsyncByPath(AssetPath, FAsyncLoadCompletedDelegate::CreateLambda([&Label,&cnt, &maxCnt, &AssetPath, &CompletedDelegate](const FName& AssetName, UObject* LoadedAsset) {
			if (AssetPath.IsValid())
			{
				if (UObject* LoadedAsset = AssetPath.ResolveObject())
				{
					cnt++;
					Get().AddLoadedAsset(AssetName, LoadedAsset);
				}
				else
				{
					UE_LOG(LogTemp, Fatal, TEXT("Failed to load asset [%s]"), *AssetPath.ToString());
				}

				if (cnt == maxCnt)
				{
					if (CompletedDelegate.IsBound())
						CompletedDelegate.Execute(Label, cnt, maxCnt);
				}
			}
		}));
	}
}

void UPUBGAssetManager::ReleaseByPath(const FSoftObjectPath& AssetPath)
{
	FName AssetName = AssetPath.GetAssetFName();
	ReleaseByName(AssetName);
}

void UPUBGAssetManager::ReleaseByName(const FName& AssetName)
{
	UPUBGAssetManager& AssetManager = Get();
	if (AssetManager.NameToLoadedAsset.Contains(AssetName))
	{
		AssetManager.NameToLoadedAsset.Remove(AssetName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Can't find loaded asset by assetName [%s]."), *AssetName.ToString());
	}
}

void UPUBGAssetManager::ReleaseByLabel(const FName& Label)
{
	UPUBGAssetManager& AssetManager = Get();
	UPUBGAssetData* LoadedAssetData = AssetManager.LoadedAssetData;
	const FAssetSet& AssetSet = LoadedAssetData->GetAssetSetByLabel(Label);

	for (const FAssetEntry& AssetEntry : AssetSet.AssetEntries)
	{
		const FName& AssetName = AssetEntry.AssetName;
		if (AssetManager.NameToLoadedAsset.Contains(AssetName))
		{
			AssetManager.NameToLoadedAsset.Remove(AssetName);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Can't find loaded asset by assetName [%s]."), *AssetName.ToString());
		}
	}
}

void UPUBGAssetManager::ReleaseAll()
{
	UPUBGAssetManager& AssetManager = Get();
	AssetManager.NameToLoadedAsset.Reset();
}

const UPUBGItemData& UPUBGAssetManager::GetItemData()
{
	return GetOrLoadTypedGameData<UPUBGItemData>(ItemDataPath);
}

UObject* UPUBGAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset maanger isn't ready yet;
		return AssetPath.TryLoad();
	}
	return nullptr;
}

bool UPUBGAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

UPrimaryDataAsset* UPUBGAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("LyraEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogTemp, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogTemp, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}

// 에셋 덩어리가 로드 되지않으면 불러와서
// Preload 라벨이 붙은 에셋을 로드한다.
void UPUBGAssetManager::LoadPreloadAssets()
{
	if (LoadedAssetData)
		return;

	UPUBGAssetData* AssetData = nullptr;
	FPrimaryAssetType PrimaryAssetType(UPUBGAssetData::StaticClass()->GetFName());
	TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete(0.f, false);
		AssetData = Cast<UPUBGAssetData>(Handle->GetLoadedAsset());
	}

	if (AssetData)
	{
		LoadedAssetData = AssetData;
		LoadSyncByLabel("Preload");
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Failed to load AssetData asset type [%s]."), *PrimaryAssetType.ToString());
	}
}

void UPUBGAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UPUBGAssetManager::AddLoadedAsset(const FName& AssetName, const UObject* Asset)
{
	if (AssetName.IsValid() && Asset)
	{
		// lock
		//FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);

		if (NameToLoadedAsset.Contains(AssetName) == false)
		{
			NameToLoadedAsset.Add(AssetName, Asset);
		}
	}
}