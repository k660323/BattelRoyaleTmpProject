// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Item/PUBGItemTemplate.h"
#include "PUBGItemFragment_Equipable.generated.h"

class UPUBGItemInstance;
class UDataAsset_StartUpDataBase;
class UPUBGFeatureEquipmentCondition;

/**
 * 
 */
UCLASS(Abstract, Const)
class PUBG_API UPUBGItemFragment_Equipable : public UPUBGItemFragment
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Equipable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	EEquipmentType EquipmentType = EEquipmentType::Count;

	UPROPERTY(EditDefaultsOnly)
	bool EquipmentImmediately = false;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UDataAsset_StartUpDataBase> BaseAbilitySet;

	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> GrantedTags;

	// Instanced : 개별 인스턴스 허용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UPUBGFeatureEquipmentCondition>> EquipFeatureConditionClasses;

	// Instanced : 개별 인스턴스 허용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UPUBGFeatureEquipmentCondition>> UnEquipFeatureConditionClasses;
};
