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
	// 아이디로 오브젝트 반환
	const UPUBGItemTemplate& FindItemTemplateByID(int32 ItemTemplateID) const;
	// 클래스로 아이디 반환
	int32 FindItemTemplateIDByClass(TSubclassOf<UPUBGItemTemplate> ItemTemplateClass) const;

	// 모든 아이템 클래스 반환
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
	// 무기
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> WeaponItemTemplateClasses;

	// 방어구
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> ArmorItemTemplateClasses;

	// 소모품
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> ConsumableItemTemplateClasses;

	// 패션
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> FashionItemTemplateClasses;

	// 무기 액서서리
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> WeaponAccItemTemplateClasses;

	// 유틸리티 
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> UtilityItemTemplateClasses;

	// 총알
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> AmmoItemTemplateClasses;

	// 기타
	UPROPERTY()
	TArray<TSubclassOf<UPUBGItemTemplate>> EtcItemTemplateClasses;
};
