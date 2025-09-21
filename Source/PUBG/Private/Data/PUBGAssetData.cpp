// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PUBGAssetData.h"
#include "UObject/ObjectSaveContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAssetData)

// "UObject/ObjectSaveContext.h" 파일 필요
void UPUBGAssetData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	AssetNameToPath.Empty();
	AssetLabelToSet.Empty();

	// 이름으로 정렬
	AssetGroupNameToSet.KeySort([](const FName& A, const FName& B)
		{
			return (A.Compare(B) < 0);
		}
	);

	for (const auto& Pair : AssetGroupNameToSet)
	{
		const FAssetSet& AssetSet = Pair.Value;
		for (FAssetEntry AssetEntry : AssetSet.AssetEntries)
		{
			FSoftObjectPath& AssetPath = AssetEntry.AssetPath;
			const FString& AssetName = AssetPath.GetAssetName();

			//if (AssetName.StartsWith(TEXT("BP_")) || AssetName.StartsWith(TEXT("B_")) || AssetName.StartsWith(TEXT("WBP_")) ||
			//	AssetName.StartsWith(TEXT("GE_")) || AssetName.StartsWith(TEXT("GA_")) || AssetName.StartsWith(TEXT("ABP_")))
			//{
			//	/*FString AssetPathString = AssetPath.GetAssetPathString();
			//	AssetPathString.Append(TEXT("_C"));*/
			//	FString AssetPathString = FString::Printf(TEXT("%s_C"), *AssetPath.ToString());
			//	AssetPath = FSoftObjectPath(AssetPathString);
			//}

			// Emplace == push_back

			// AssetNameToPath 캐싱 (FAssetEntry이름으로 에셋 경로 묶음) (직접 접근)
			AssetNameToPath.Emplace(AssetEntry.AssetName, AssetPath);
			
			// AssetLabelToSet 캐싱 (라벨로 FAssetSet 묶음) (덩어리 접근)
			for (const FName& Label : AssetEntry.AssetLables)
			{
				AssetLabelToSet.FindOrAdd(Label).AssetEntries.Emplace(AssetEntry);
			}
		}
	}
}


FSoftObjectPath UPUBGAssetData::GetAssetPathByName(const FName& AssetName)
{
	FSoftObjectPath* AssetPath = AssetNameToPath.Find(AssetName);
	ensureAlwaysMsgf(AssetPath, TEXT("Can't find Asset Path from Asset Name [%s]."), *AssetName.ToString());
	
	return *AssetPath;
}

const FAssetSet& UPUBGAssetData::GetAssetSetByLabel(const FName& Label)
{
	const FAssetSet* AssetSet = AssetLabelToSet.Find(Label);
	ensureAlwaysMsgf(AssetSet, TEXT("Can't find Asset Set from Label [%s]."), *Label.ToString());
	return *AssetSet;
}
