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
	// ��� -> �κ��丮 (����, ��ô)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToInventory(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// �κ��丮 -> ��� (����, ��ô)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipment(UPUBGInventoryManagerComponent* FromInventoryManager, int32 FromItemIndex, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	
	// �κ��丮 -> ��� ������ (�ӽ�)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InventoryToEquipGunAcc(UPUBGInventoryManagerComponent* FromInventoryManager, int32 FromItemIndex, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType);

	// ���(������) -> �κ��丮 (�ӽ�)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipGunAccToInventory(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// ���(������) <-> ���(������) (�ӽ�)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipGunAccToEquipGunAcc(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// ��� <-> ��� (�ֹ��� ��ü)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_EquipmentToEquipment(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);
	
public:
	// ��� -> ��� ������
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromEquipment(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType);
	
	// �κ��丮 -> ��� ������
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromInventory(UPUBGInventoryManagerComponent* FromInventoryManager, int32 ItemIndex, int32 DropCount);

	// ���(������) -> ��� ������ (�ӽ�)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItemFromEquipGunAcc(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType);

public:
	// ��� ������ -> ��� ������
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickupItemToEquipGunAcc(APUBGPickupableItemBase* FromPickupableItem, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType, UPUBGInventoryManagerComponent* ToInventoryManager);

	// ��� ������ -> ��� (�ֹ���1, �ֹ���2 ����)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickupItemToEquipment(APUBGPickupableItemBase* FromPickupableItem, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType);

	// ��� ������ -> �κ��丮 (���� �� �ʿ䰡 �ֳ�? TryPickupItem�� ����� ����� �ְ� ������, �巡�� ����� ȣ���� �Լ��� �����ؾߵ�)
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickupItemToInventory(APUBGPickupableItemBase* FromPickupableItem, UPUBGInventoryManagerComponent* ToInventoryManager);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool TryPickupItem(APUBGPickupableItemBase* PickupableItem);

	// ��ɾ�� ������ �ִ� �Լ�
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
