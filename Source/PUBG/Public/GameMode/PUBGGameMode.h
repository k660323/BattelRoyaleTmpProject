// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "GameFramework/GameMode.h"
#include "PUBGGameMode.generated.h"

class UPUBGItemInstance;
class APUBGPickupableItemBase;

/**
 * 
 */
UCLASS()
class PUBG_API APUBGGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	APUBGGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void GenerateItems();

	UPUBGItemInstance* NativeCreateItemInstance(int32 ItemTemplateID);
	APUBGPickupableItemBase* NativeSpawnPickupableItemByID(int32 ItemTemplateID, int32 ItemCount = 1);
	APUBGPickupableItemBase* NativeSpawnPickupableItemByItemInstance(UPUBGItemInstance* ItemInstance, int32 ItemCount = 1);
};
