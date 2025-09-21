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

	// 장착가능한 무기타입에 따른 매쉬 맵 (태그로 변경)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UPUBGItemTemplate>, TSoftObjectPtr<UStaticMesh>> GunAccMeshMap;

	// 추가 탄창 (태그와 카운트로 즉 맵으로 변경)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UPUBGItemTemplate>, int32> ExtraClipSizeMap;

	// 소음 여부, 초기 섬광, 배율
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Weapon"))
	TArray<FGameplayTag> EquipAccTags;

	// 장전 속도, 반동 회복, 수직 반동, 수평 반동, 정 조준 시간
	// 비조준 사격 정확도, 조준 사격 정확도, 초탄 반동
	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FStatSet> StatSets;
};
