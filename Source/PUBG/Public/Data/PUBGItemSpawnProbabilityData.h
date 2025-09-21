// Kakanu Portfolio Project

#pragma once

#include "Engine/DataAsset.h"
#include "PUBGTypes/PUBGItemProbability.h"
#include "PUBGItemSpawnProbabilityData.generated.h"

class UDataTable;
class UPUBGItemData;
class UPUBGItemTemplate;
class UPUBGItemFragment;
class UPUBGWorldItemSpawnData;

USTRUCT(BlueprintType)
struct FPUBGItemModelData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPUBGItemModelProbability> ItemModelProbabilitySet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TotalPercent = 0.0f;
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemSpawnProbabilityData : public UDataAsset
{
	GENERATED_BODY()
	
public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA
	
	const FPUBGItemModelData* GetItemModelProbability(TSubclassOf<UPUBGItemFragment> FragmentClass, int32 ItemType) const;
	
	int32 RandomItemSpawnCount() const;
	EItemType SelectRandomMajorCategoriesItemType() const;
	TPair<UEnum*, int32> SelectRandomMiddleClassificationItemType(EItemType ItemType) const;
	int32 SelectRandomSubcategoriesItemID(TPair<UEnum*, int32>& EnumInfoPair) const;

	UPUBGWorldItemSpawnData* GetWorldItemSpawnData() const;
	const UDataTable* GetItemSpawnCountProbability() const;
	const UDataTable* GetMajorCategoriesItemTypeDataTable() const;
	const UDataTable* GetMiddleClassificationItemTypeDataTable(TSubclassOf<UPUBGItemFragment> FragmentClass) const;

private:
#if WITH_EDITORONLY_DATA
	void ItemDuplicateCheck(UPUBGItemData* LoadedItemData, UEnum* DestEnum, int32& EnumNumber, FPUBGItemModelData& DestItemModelData, OUT TArray<FPUBGItemModelProbability>& TempArray, OUT TSet<int32>& TempItemIDSet);

	void NewItemAdd(UPUBGItemData* LoadedItemData, const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses, int32& EnumNumber, FPUBGItemModelData& DestItemModelData, TArray<FPUBGItemModelProbability>& TempArray, TSet<int32>& ItemNumberSet);

	void CheckAndAdd(UPUBGItemData* LoadedItemData, const TArray<TSubclassOf<UPUBGItemTemplate>>& ItemTemplateClasses, UEnum* DestEnum, int32& EnumNumber, FPUBGItemModelData& DestItemModelData, TArray<FPUBGItemModelProbability>& TempArray, TSet<int32>& TempItemIDSet);

	void CalculationTotalPercent(FPUBGItemModelData& DestItemModelData);
#endif // WITH_EDITORONLY_DATA

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "PreSaveOption", meta = (AllowPrivateAccess = "true"))
	bool IsClear = false;

	UPROPERTY(EditDefaultsOnly, Category = "PreSaveOption", meta = (AllowPrivateAccess = "true"))
	bool IsCheckAndAddNewItem = false;
#endif // WITH_EDITORONLY_DATA

	// ������ ������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UPUBGItemData> ItemData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SpawnTransform", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPUBGWorldItemSpawnData* WorldItemSpawnData;

	// ���� ������ ī����
	UPROPERTY(EditDefaultsOnly, Category = "Item|SpawnCount", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemSpawnCountProbability;

	// ��з� ������ Ȯ��
	UPROPERTY(EditDefaultsOnly, Category = "Item|MajorCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemTypeProbability;

	// �ߺз� ������ Ȯ��
	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* GunAccTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* ArmorTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* ConsumptionTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* FashionTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* UtilityTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* PayloadTypeProbability;

	UPROPERTY(EditDefaultsOnly, Category = "Item|MiddleClassification", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDataTable* EtcTypeProbability;

	// �Һз� ������ Ȯ��

	// ��, ����, ��ô (�̱���)
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData GunWeaponItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData SubGunWeaponItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData MeleeWeaponItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData ThrowWeaponItemModelData;

	// ����, �ѱ�, ������, źâ, ���Ӹ���
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData SightItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData MuzzleItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData ForegripItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData MagItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData ButtstockItemModelData;

	// ���, ����
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData HelmetItemModelData;
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData VestItemModelData;

	// ��, ���ǰ�
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData HealItemModelData;
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData DrinkItemModelData;

	// ��
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData ClothTopItemModelData;
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData ClothBottomItemModelData;
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData WholeItemModelData;
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData ShoesItemModelData;

	// ����, ��Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData BackpackItemModelData;

	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData BeltItemModelData;
	
	// �Ѿ�
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData AmmoItemModelData;

	// (�̱���)
	UPROPERTY(EditDefaultsOnly, Category = "Item|SubCategories", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPUBGItemModelData EtcItemModelData;

};
