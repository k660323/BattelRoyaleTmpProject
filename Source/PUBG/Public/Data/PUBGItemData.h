// Kakanu Portfolio Project

#pragma once

#include "Engine/DataAsset.h"
#include "PUBGItemData.generated.h"

class UPUBGItemTemplate;

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const UPUBGItemData& Get();

public:
#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

public:
	// ���̵�� ������Ʈ ��ȯ
	const UPUBGItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	// Ŭ������ ���̵� ��ȯ
	int32 FindItemTemplateIDByClass(TSubclassOf<UPUBGItemTemplate> ItemTemplateClass) const;

	// ��� ������ Ŭ���� ��ȯ
	void GetAllItemTemplateClasses(TArray<TSubclassOf<UPUBGItemTemplate>>& OutItemTemplateClasses) const;

	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetWeaponItemTemplateClasses() const { return WeaponItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetArmorItemTemplateClasses() const { return ArmorItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetConsumableItemTemplateClasses() const { return ConsumableItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetFashionItemTemplateClasses() const { return FashionItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetWeaponAccItemTemplateClasses() const { return WeaponAccItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetUtilityItemTemplateClasses() const { return UtilityItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetAmmoItemTemplateClasses() const { return AmmoItemTemplateClasses; }
	const TArray<TSubclassOf<UPUBGItemTemplate>>& GetEtcItemTemplateClasses() const { return EtcItemTemplateClasses; }

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<UPUBGItemTemplate>> ItemTemplateIDToClass;


	UPROPERTY()
	TMap<TSubclassOf<UPUBGItemTemplate>, int32> ItemTemplateClassToID;

private:
	// ����
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> WeaponItemTemplateClasses;

	// ��
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> ArmorItemTemplateClasses;

	// �Ҹ�ǰ
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> ConsumableItemTemplateClasses;

	// �м�
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> FashionItemTemplateClasses;

	// ���� �׼�����
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> WeaponAccItemTemplateClasses;

	// ��ƿ��Ƽ 
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> UtilityItemTemplateClasses;

	// �Ѿ�
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> AmmoItemTemplateClasses;

	// ��Ÿ
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> EtcItemTemplateClasses;
};
