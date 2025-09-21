// Kakanu Portfolio Project

#pragma once

#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "PUBGItemFragment_Weapon.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Weapon : public UPUBGItemFragment_Attachment
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Weapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UPUBGItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType = EWeaponType::Count;

	UPROPERTY(EditDefaultsOnly)
	EWeaponClassType WeaponClassType = EWeaponClassType::Count;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TMap<EGunAccType, bool> UseAccMap;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> BigIcon;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Thumbnail;
	
	UPROPERTY(EditDefaultsOnly)
	float WalkSpeedPer;

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FStatSet> StatSets;
};
