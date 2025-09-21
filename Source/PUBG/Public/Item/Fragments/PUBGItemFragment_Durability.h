// Kakanu Portfolio Project

#pragma once

#include "GameplayTags.h"
#include "Item/PUBGItemTemplate.h"
#include "PUBGItemFragment_Durability.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Durability : public UPUBGItemFragment
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Durability(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UPUBGItemInstance* ItemInstance) const override;

	virtual bool GetDefaultStackValueByTag(FGameplayTag& InTag, int32& Value) const;

public:
	UPROPERTY(EditDefaultsOnly)
	bool IsAutoDestroy = true;

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FInitStatSet> StatSets;
};
