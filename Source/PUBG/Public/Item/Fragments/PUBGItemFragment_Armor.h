// Kakanu Portfolio Project

#pragma once

#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "PUBGItemFragment_Armor.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Armor : public UPUBGItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Armor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UPUBGItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EArmorType ArmorType = EArmorType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> ArmorMesh;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Thumbnails;

	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TArray<FStatSet> StatSets;
};
