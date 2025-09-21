// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PUBGItemFunctionLibrary.generated.h"

class UPUBGItemTemplate;
class UPUBGItemInstance;
class APUBGPickupableItemBase;
/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGItemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 임시
	static UPUBGItemInstance* NativeCreateItemInstance(int32 ItemTemplateID);

	UFUNCTION(BlueprintCallable, Category = "PUBG/ItemFunctionLibrary", meta = (DisplayName = "CreateItemInstance"))
	static UPUBGItemInstance* BP_CreateItemInstance(int32 ItemTemplateID);

	static APUBGPickupableItemBase* NativeCreatePickupableItemBase(UObject* WorldContextObject, int32 ItemTemplateID, int32 ItemCount = 1);

	UFUNCTION(BlueprintCallable, Category = "PUBG/ItemFunctionLibrary", meta = (DisplayName = "CreatePickupableItemBase",WorldContext = "WorldContextObject"))
	static APUBGPickupableItemBase* BP_CreatePickupableItemBase(UObject* WorldContextObject, int32 ItemTemplateID, int32 ItemCount = 1);

	// 기능 확인용
	static void NativeEquipEquipItem(int32 ItemTemplateID, AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "PUBG/ItemFunctionLibrary", meta = (DisplayName = "EquipItem"))
	static void BP_EquipEquipItem(int32 ItemTemplateID, AActor* Actor);

	static void NativeUnequipEquipItem(EEquipmentSlotType EquipmentSlotType, AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "PUBG/ItemFunctionLibrary", meta = (DisplayName = "UnequipItem"))
	static void BP_UnequipEquipItem(EEquipmentSlotType EquipmentSlotType, AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "PUBG/ItemFunctionLibrary", meta = (DisplayName = "AllItemSpawn",WorldContext="WorldContextObject"))
	static TArray<APUBGPickupableItemBase*> BP_AllItemSpawn(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "PUBG/ItemFunctionLibrary", meta = (DisplayName = "SpawnItem", WorldContext = "WorldContextObject"))
	static void GenerateItems(UObject* WorldContextObject);

}; 
