// Kakanu Portfolio Project

#pragma once

#include "Components/ControllerComponent.h"
#include "PUBGItemManagerComponent.generated.h"

class APUBGPickupableItemBase;
class UPUBGEquipmentManagerComponent;
class UPUBGInventoryManagerComponent;
class UPUBGItemInstance;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PUBG_API UPUBGItemManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGItemManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// 장비 -> 인벤토리 (근접, 투척)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToInventory(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// 인벤토리 -> 장비 (근접, 투척)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipment(UPUBGInventoryManagerComponent* FromInventoryManager, int32 FromItemIndex, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	
	// 인벤토리 -> 장비 부착물 (임시)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipGunAcc(UPUBGInventoryManagerComponent* FromInventoryManager, int32 FromItemIndex, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType);

	// 장비(부착물) -> 인벤토리 (임시)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipGunAccToInventory(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// 장비(부착물) <-> 장비(부착물) (임시)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipGunAccToEquipGunAcc(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// 장비 <-> 장비 (주무기 교체)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToEquipment(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	
public:
	// 장비 -> 드랍 아이템
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromEquipment(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);
	
	// 인벤토리 -> 드랍 아이템
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromInventory(UPUBGInventoryManagerComponent* FromInventoryManager, int32 ItemIndex, int32 DropCount);

	// 장비(부착물) -> 드랍 아이템 (임시)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromEquipGunAcc(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType);

public:
	// 드랍 아이템 -> 장비 부착물
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickupItemToEquipGunAcc(APUBGPickupableItemBase* FromPickupableItem, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// 드랍 아이템 -> 장비 (주무기1, 주무기2 선택)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickupItemToEquipment(APUBGPickupableItemBase* FromPickupableItem, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

	// 드랍 아이템 -> 인벤토리 (구현 할 필요가 있나? TryPickupItem이 기능을 대신해 주고 있으나, 드래그 드랍시 호출할 함수가 존재해야됨)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickupItemToInventory(APUBGPickupableItemBase* FromPickupableItem, UPUBGInventoryManagerComponent* ToInventoryManager);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryPickupItem(APUBGPickupableItemBase* PickupableItem);

	// 명령어로 아이템 넣는 함수
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryAddItem(UPUBGItemInstance* InItemInstance, int32 InItemCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryDropItem(UPUBGItemInstance* ItemInstance, int32 ItemCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	APUBGPickupableItemBase* SpawnPickupableItem(UPUBGItemInstance* ItemInstance, int32 Count = 1) const;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddAllowedComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveAllowedComponent(UActorComponent* ActorComponent);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool IsAllowedComponent(UActorComponent* ActorComponent);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPUBGEquipmentManagerComponent* GetPawnEquipmentManager() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPUBGInventoryManagerComponent* GetPawnInventoryManager() const;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UActorComponent>> AllowedComponents;

	UPROPERTY()
	float AddtionalRange = 10;
};
