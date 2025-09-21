// Kakanu Portfolio Project

#pragma once

#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "PUBGItemFragment_GunAcc.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_GunAcc : public UPUBGItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_GunAcc(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UPUBGItemInstance* ItemInstance) const override;

	bool IsEquipableWeaponItem(TSubclassOf<UPUBGItemTemplate> WeaponItemTemplate) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EGunAccType GunAccType = EGunAccType::Count;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	// ���������� ����Ÿ�Կ� ���� �Ž� �� (�±׷� ����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UPUBGItemTemplate>, TSoftObjectPtr<UStaticMesh>> GunAccMeshMap;

	// �߰� źâ (�±׿� ī��Ʈ�� �� ������ ����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UPUBGItemTemplate>, int32> ExtraClipSizeMap;

	// ���� ����, �ʱ� ����, ����
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Weapon"))
	TArray<FGameplayTag> EquipAccTags;

	// ���� �ӵ�, �ݵ� ȸ��, ���� �ݵ�, ���� �ݵ�, �� ���� �ð�
	// ������ ��� ��Ȯ��, ���� ��� ��Ȯ��, ��ź �ݵ�
	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FStatSet> StatSets;
};
