// Kakanu Portfolio Project

#pragma once

#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "PUBGItemFragment_Attachment.generated.h"

class UPUBGLinkedAnimLayer;
class APUBGEquipmentBase;
class UPUBGAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FPUBGWeaponAttachInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APUBGEquipmentBase> SpawnWeaponClass;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TMap<FGameplayTag, FName> AttachSocketMap;

	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransformOffset;
};

/**
 * 
 */
UCLASS(Abstract, Const)
class PUBG_API UPUBGItemFragment_Attachment : public UPUBGItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Attachment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FName GetActiveEquipmentAttachSocket(UPUBGAbilitySystemComponent* ASC) const;
	TSoftObjectPtr<UAnimMontage> GetActiveEquipmentMontageByStateTag(UPUBGAbilitySystemComponent* ASC) const;
	TSoftObjectPtr<UAnimMontage> GetUnActiveEquipmentMontageByStateTag(UPUBGAbilitySystemComponent* ASC) const;

public:
	UPROPERTY(EditDefaultsOnly)
	FPUBGWeaponAttachInfo AttachInfo;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TMap<FGameplayTag, TSoftObjectPtr<UAnimMontage>> EquipMontageMap;

	UPROPERTY(EditDefaultsOnly, EditFixedSize)
	TMap<FGameplayTag,TSoftObjectPtr<UAnimMontage>> UnequipMontageMap;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPUBGLinkedAnimLayer> AttachmentAnimLayerToLinkClass;
};
