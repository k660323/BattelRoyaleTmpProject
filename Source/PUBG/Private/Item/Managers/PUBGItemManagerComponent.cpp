// Kakanu Portfolio Project


#include "Item/Managers/PUBGItemManagerComponent.h"
#include "Actors/PUBGEquipmentBase.h"
#include "Actors/PUBGPickupableItemBase.h"
#include "Interaction/PUBGPickupable.h"
#include "Item/PUBGItemInstance.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/Managers/PUBGInventoryManagerComponent.h"
#include "Item/FeatureConditions/EquipmentCondition/PUBGFeatureEquipmentCondition.h"
#include "Physics/PUBGPhysicsFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "System/PUBGAssetManager.h"
#include "AbilitySystemInterface.h"
#include "Data/PUBGItemData.h"
#include "Interaction/Abilities/PUBGGA_Interact.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemManagerComponent)

// Sets default values for this component's properties
UPUBGItemManagerComponent::UPUBGItemManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UPUBGItemManagerComponent::Server_EquipmentToInventory_Implementation(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UPUBGInventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromEquipmentManager) == false || IsAllowedComponent(ToInventoryManager) == false)
	{
		return;
	}

	if (FromEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	if (FromEquipmentSlotType == EEquipmentSlotType::Etc || FromEquipmentSlotType == EEquipmentSlotType::Throw)
	{
		TArray<FPUBGEquipmentEntry> Entries = FromEquipmentManager->GetAllEntries();
		FPUBGEquipmentEntry& Entry = Entries[(int32)FromEquipmentSlotType];
		int32 Count = 1;

		if (ToInventoryManager->CanAddItem(Entry.ItemInstance, Count))
		{
			UPUBGItemInstance* RemovedItemInstance = FromEquipmentManager->Unequipment(FromEquipmentSlotType);
			int32 RemoveItemCount = 1;

			if (IsValid(RemovedItemInstance))
			{
				ToInventoryManager->TryAddItem(Entry.ItemInstance, RemoveItemCount);
			}
		}
	}
}

void UPUBGItemManagerComponent::Server_InventoryToEquipment_Implementation(UPUBGInventoryManagerComponent* FromInventoryManager, int32 FromItemIndex, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromInventoryManager) == false || IsAllowedComponent(ToEquipmentManager) == false)
	{
		return;
	}

	FPUBGInventoryEntry& FromInventoryEntry = FromInventoryManager->GetEntry(FromItemIndex);
	UPUBGItemInstance* FromItemInstance = FromInventoryEntry.GetItemInstance();
	if (IsValid(FromItemInstance) == false)
	{
		return;
	}

	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	if (ToEquipmentSlotType == EEquipmentSlotType::Etc || ToEquipmentSlotType == EEquipmentSlotType::Throw)
	{
		// ex) �κ��丮 ����ź -> ��� ��ô ���� (����)
		//	   �κ��丮 ����ź -> �� ���
		
		FPUBGEquipmentEntry& ToEquipmentEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
		UPUBGItemInstance* ToItemInstance = ToEquipmentEntry.GetItemInstance();
		// ��� �����
		if (IsValid(ToItemInstance))
		{
			if (FromItemInstance->GetItemTemplateID() == ToItemInstance->GetItemTemplateID())
			{
				return;
			}

			// �κ��丮������ �ϳ� ����
			UPUBGItemInstance* RemovedItemInstanceFrom = FromInventoryManager->TryRemoveItem(FromItemIndex, 1);
			// ���� ��� ����
			UPUBGItemInstance* RemovedItemInstanceTo = ToEquipmentManager->Unequipment(ToEquipmentSlotType);

			// �κ��丮 �������� -> ����
			ToEquipmentManager->Equipment(ToEquipmentSlotType, RemovedItemInstanceFrom);

			// ��� �������� -> �κ��丮 ������
			int32 TmpItemCount = 1;
			if (FromInventoryManager->CanAddItem(RemovedItemInstanceTo, TmpItemCount))
			{
				int32 ItemCount = 1;
				FromInventoryManager->TryAddItem(RemovedItemInstanceTo, ItemCount);
			}
			// �Ⱦ� ���������� ��ȯ
			else
			{
				TryDropItem(RemovedItemInstanceTo, 1);
			}
		}
		// ��� �������
		else
		{
			//int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemIndex);
			// ������ ���� (������ �ϳ����� ����)
			int32 MovableCount = FromInventoryManager->CanRemoveItem(FromItemIndex, 1);
			if (MovableCount > 0)
			{
				// �κ����� ������ �ϳ���������
				UPUBGItemInstance* RemovedItemInstance = FromInventoryManager->TryRemoveItem(FromItemIndex, 1);
				// ���â�� �ֱ�
				if (RemovedItemInstance)
				{
					ToEquipmentManager->Equipment(ToEquipmentSlotType, RemovedItemInstance);
				}
			}
		}
	}
}

void UPUBGItemManagerComponent::Server_InventoryToEquipGunAcc_Implementation(UPUBGInventoryManagerComponent* FromInventoryManager, int32 FromItemIndex, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromInventoryManager == nullptr || ToEquipmentManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromInventoryManager) == false || IsAllowedComponent(ToEquipmentManager) == false)
	{
		return;
	}

	FPUBGInventoryEntry& FromInventoryEntry = FromInventoryManager->GetEntry(FromItemIndex);
	UPUBGItemInstance* FromItemInstance = FromInventoryEntry.GetItemInstance();
	if (IsValid(FromItemInstance) == false)
	{
		return;
	}

	const UPUBGItemFragment_GunAcc* GunAccFragment = FromItemInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();

	if (GunAccFragment == nullptr)
	{
		return;
	}

	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}


	// TODO : ���� �������� Ȯ�� �Լ�

	FPUBGEquipmentEntry& ToEquipmentEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
	UPUBGItemInstance* ToEquipmentItemInstance = ToEquipmentEntry.GetItemInstance();
	APUBGEquipmentBase* ToEquipmentActor = ToEquipmentEntry.SpawnedEquipmentActor;

	if (IsValid(ToEquipmentItemInstance) == false || IsValid(ToEquipmentActor) == false)
	{
		return;
	}

	if (ToAccType == EGunAccType::Count)
	{
		return;
	}

	if (ToEquipmentActor->IsEquipableWeaponAccTypeSocket(ToAccType) == false)
	{
		return;
	}

	// ��� �׼����� �����
	if (ToEquipmentActor->IsValidWeaponAcc(ToAccType))
	{
		UPUBGItemInstance* RemovedItemInstance = ToEquipmentActor->UnEquipmentWeaponAcc(ToAccType);
		// ��� �������� -> �κ��丮 ������
		int32 TmpItemCount = 1;
		if (FromInventoryManager->CanAddItem(RemovedItemInstance, TmpItemCount))
		{
			int32 ItemCount = 1;
			FromInventoryManager->TryAddItem(RemovedItemInstance, ItemCount);
		}
		// �Ⱦ� ���������� ��ȯ
		else
		{
			TryDropItem(RemovedItemInstance, 1);
		}
	}

	// ��� �׼����� ����
	ToEquipmentActor->EquipmentWeaponAcc(ToAccType, FromItemInstance);
}

void UPUBGItemManagerComponent::Server_EquipGunAccToInventory_Implementation(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType, UPUBGInventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromEquipmentManager == nullptr || ToInventoryManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromEquipmentManager) == false || IsAllowedComponent(ToInventoryManager) == false)
	{
		return;
	}

	FPUBGEquipmentEntry& FromEquipmentEntry = FromEquipmentManager->GetEntry(FromEquipmentSlotType);
	UPUBGItemInstance* FromEquipmentItemInstance = FromEquipmentEntry.GetItemInstance();
	APUBGEquipmentBase* FromEquipmentActor = FromEquipmentEntry.SpawnedEquipmentActor;

	if (IsValid(FromEquipmentItemInstance) == false || IsValid(FromEquipmentActor) == false)
	{
		return;
	}

	if (FromAccType == EGunAccType::Count)
	{
		return;
	}

	if (FromEquipmentActor->IsEquipableWeaponAccTypeSocket(FromAccType) == false)
	{
		return;
	}

	UPUBGItemInstance* ItemInstance = FromEquipmentActor->GetWeaponAccItemInstance(FromAccType);

	int32 Count = 1;
	if (ToInventoryManager->CanAddItem(ItemInstance, Count))
	{
		UPUBGItemInstance* RemovedItemInstance = FromEquipmentActor->UnEquipmentWeaponAcc(FromAccType);
		int32 RemoveItemCount = 1;

		ToInventoryManager->TryAddItem(RemovedItemInstance, RemoveItemCount);
	}
}

void UPUBGItemManagerComponent::Server_EquipGunAccToEquipGunAcc_Implementation(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType, UPUBGInventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr || ToInventoryManager == nullptr)
	{
		return;
	}


	if (IsAllowedComponent(FromEquipmentManager) == false || IsAllowedComponent(ToEquipmentManager) == false || IsAllowedComponent(ToInventoryManager) == false)
	{
		return;
	}

	if (FromAccType == EGunAccType::Count || ToAccType == EGunAccType::Count)
	{
		return;
	}

	if (FromAccType != ToAccType)
	{
		return;
	}

	FPUBGEquipmentEntry& FromEquipmentEntry = FromEquipmentManager->GetEntry(FromEquipmentSlotType);
	UPUBGItemInstance* FromEquipmentItemInstance = FromEquipmentEntry.GetItemInstance();
	APUBGEquipmentBase* FromEquipmentActor = FromEquipmentEntry.SpawnedEquipmentActor;

	FPUBGEquipmentEntry& ToEquipmentEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
	UPUBGItemInstance* ToEquipmentItemInstance = ToEquipmentEntry.GetItemInstance();
	APUBGEquipmentBase* ToEquipmentActor = ToEquipmentEntry.SpawnedEquipmentActor;

	if (IsValid(FromEquipmentItemInstance) == false || IsValid(ToEquipmentItemInstance) == false)
	{
		return;
	}

	if (IsValid(FromEquipmentActor) == false || IsValid(ToEquipmentActor) == false)
	{
		return;
	}

	bool bFromOpenSocket = FromEquipmentActor->IsEquipableWeaponAccTypeSocket(ToAccType);
	bool bToOpenSocket = ToEquipmentActor->IsEquipableWeaponAccTypeSocket(FromAccType);

	// ������ ���ʸ� ���������� ��ŵ
	if (bFromOpenSocket == false || bToOpenSocket == false)
	{
		return;
	}
	
	UPUBGItemInstance* FromWeaponAccInstance = FromEquipmentActor->GetWeaponAccItemInstance(FromAccType);
	const UPUBGItemFragment_GunAcc* FromWeaponAccFragment = FromWeaponAccInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();
	const UPUBGItemTemplate& FromWeaponTemplate = UPUBGItemData::Get().FindItemTemplateByID(FromWeaponAccInstance->GetItemTemplateID());

	// �������� �������ν��Ͻ��� ������ ��ŵ
	if (IsValid(FromWeaponAccInstance) == false)
	{
		return;
	}

	UPUBGItemInstance* ToWeaponAccInstance = ToEquipmentActor->GetWeaponAccItemInstance(ToAccType);
	const UPUBGItemFragment_GunAcc* ToWeaponAccFragment = ToWeaponAccInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();
	const UPUBGItemTemplate& ToWeaponTemplate = UPUBGItemData::Get().FindItemTemplateByID(ToWeaponAccInstance->GetItemTemplateID());
	
	bool bToEquipWeaponAccByFrom = FromWeaponAccFragment->GunAccMeshMap.Contains(ToWeaponTemplate.GetClass());
	bool bFromEquipWeaponAccByTo = ToWeaponAccFragment->GunAccMeshMap.Contains(FromWeaponTemplate.GetClass());

	// ��ǥ������ �������� ������ ���
	if (IsValid(ToWeaponAccInstance))
	{
		// ���� ȣȯ�� �Ǵ°�� ����
		if (bToEquipWeaponAccByFrom && bFromEquipWeaponAccByTo)
		{
			FromWeaponAccInstance = FromEquipmentActor->UnEquipmentWeaponAcc(FromAccType);
			ToWeaponAccInstance = ToEquipmentActor->UnEquipmentWeaponAcc(ToAccType);
			FromEquipmentActor->EquipmentWeaponAcc(ToAccType, ToWeaponAccInstance);
			ToEquipmentActor->EquipmentWeaponAcc(FromAccType, FromWeaponAccInstance);
		}
		else if (bToEquipWeaponAccByFrom)
		{
			FromWeaponAccInstance = FromEquipmentActor->UnEquipmentWeaponAcc(FromAccType);
			ToWeaponAccInstance = ToEquipmentActor->UnEquipmentWeaponAcc(ToAccType);
			ToEquipmentActor->EquipmentWeaponAcc(FromAccType, FromWeaponAccInstance);

			// �κ� �Ǵ� �ٴ����� (To)
			// ��� �׼����� �����
			UPUBGItemInstance* RemovedItemInstance = ToWeaponAccInstance;
			// ��� �������� -> �κ��丮 ������
			int32 TmpItemCount = 1;
			if (ToInventoryManager->CanAddItem(RemovedItemInstance, TmpItemCount))
			{
				int32 ItemCount = 1;
				ToInventoryManager->TryAddItem(RemovedItemInstance, ItemCount);
			}
			// �Ⱦ� ���������� ��ȯ
			else
			{
				TryDropItem(RemovedItemInstance, 1);
			}
		}
	}
	else
	{
		if (bToEquipWeaponAccByFrom)
		{
			FromWeaponAccInstance = FromEquipmentActor->UnEquipmentWeaponAcc(FromAccType);
			ToEquipmentActor->EquipmentWeaponAcc(FromAccType, FromWeaponAccInstance);
		}
	}
}

void UPUBGItemManagerComponent::Server_EquipmentToEquipment_Implementation(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromEquipmentManager == nullptr || ToEquipmentManager == nullptr)
	{
		return;
	}

	if (FromEquipmentSlotType == EEquipmentSlotType::Count || ToEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	if (IsAllowedComponent(FromEquipmentManager) == false || IsAllowedComponent(ToEquipmentManager) == false)
	{
		return;
	}

	// �ֹ��� ��ü
	if ((FromEquipmentSlotType == EEquipmentSlotType::Main && ToEquipmentSlotType == EEquipmentSlotType::Main2) ||
		(FromEquipmentSlotType == EEquipmentSlotType::Main2 && ToEquipmentSlotType == EEquipmentSlotType::Main))
	{
		FPUBGEquipmentEntry& FromEntry =  FromEquipmentManager->GetEntry(FromEquipmentSlotType);
		UPUBGItemInstance* FromItemInstance = FromEntry.GetItemInstance();
		FPUBGEquipmentEntry& ToEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
		UPUBGItemInstance* ToItemInstance = ToEntry.GetItemInstance();

		// �ι��� ����
		if (FromItemInstance && ToItemInstance)
		{
			// Equipment �Լ��� ���� �������� ������ �����ϰ� �����Ѵ�.
			FromEquipmentManager->Equipment(ToEquipmentSlotType, ToItemInstance);
			ToEquipmentManager->Equipment(FromEquipmentSlotType, FromItemInstance);
		}
		// ���ʸ� ����
		else if (FromItemInstance)
		{
			FromEquipmentManager->Unequipment(FromEquipmentSlotType);
			ToEquipmentManager->Equipment(FromEquipmentSlotType, FromItemInstance);
		}
		else if (ToItemInstance)
		{
			ToEquipmentManager->Unequipment(ToEquipmentSlotType);
			ToEquipmentManager->Equipment(ToEquipmentSlotType, ToItemInstance);
		}
	}
}

void UPUBGItemManagerComponent::Server_DropItemFromEquipment_Implementation(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromEquipmentManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromEquipmentManager) == false)
	{
		return;
	}

	if (FromEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	FPUBGEquipmentEntry& FromEntry = FromEquipmentManager->GetEntry(FromEquipmentSlotType);
	APUBGEquipmentBase* EquipmentActor = FromEntry.GetEquipmentActor();
	UPUBGItemInstance* FromItemInstance = FromEntry.GetItemInstance();

	if (IsValid(FromItemInstance) == false)
	{
		return;
	}

	// ���� ���� ����
	if (const UPUBGItemFragment_Weapon* WeaponFragment = FromItemInstance->FindFragmentByClass<UPUBGItemFragment_Weapon>())
	{
		if (EquipmentActor)
		{
			int32 Count = (int32)EGunAccType::Count;
			for (int32 i = 0; i < Count; i++)
			{
				UPUBGItemInstance* RemovedItemInstance = EquipmentActor->UnEquipmentWeaponAcc((EGunAccType)i);
				TryDropItem(RemovedItemInstance, 1);
			}
		}
	}

	if (TryDropItem(FromItemInstance, 1))
	{
		FromEquipmentManager->Unequipment(FromEquipmentSlotType);
	}
}

void UPUBGItemManagerComponent::Server_DropItemFromInventory_Implementation(UPUBGInventoryManagerComponent* FromInventoryManager, int32 ItemIndex, int32 DropCount)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromInventoryManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromInventoryManager) == false)
	{
		return;
	}

	FPUBGInventoryEntry& FromEntry = FromInventoryManager->GetEntry(ItemIndex);
	int32 ItemCount = FromEntry.GetItemCount();
	DropCount = FromInventoryManager->CanRemoveItem(ItemIndex, DropCount);

	// ������ �ִ� ī���Ͱ� ����
	if (DropCount > 0)
	{
		UPUBGItemInstance* RemovedItemInstance = FromInventoryManager->TryRemoveItem(ItemIndex, DropCount);
		TryDropItem(RemovedItemInstance, DropCount);
	}
}

void UPUBGItemManagerComponent::Server_DropItemFromEquipGunAcc_Implementation(UPUBGEquipmentManagerComponent* FromEquipmentManager, EEquipmentSlotType FromEquipmentSlotType, EGunAccType FromAccType)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromEquipmentManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(FromEquipmentManager) == false)
	{
		return;
	}

	FPUBGEquipmentEntry& FromEquipmentEntry = FromEquipmentManager->GetEntry(FromEquipmentSlotType);
	UPUBGItemInstance* FromEquipmentItemInstance = FromEquipmentEntry.GetItemInstance();
	APUBGEquipmentBase* FromEquipmentActor = FromEquipmentEntry.SpawnedEquipmentActor;

	if (IsValid(FromEquipmentItemInstance) == false || IsValid(FromEquipmentActor) == false)
	{
		return;
	}

	if (FromAccType == EGunAccType::Count)
	{
		return;
	}

	UPUBGItemInstance* RemovedItemInstance = FromEquipmentActor->UnEquipmentWeaponAcc(FromAccType);
	TryDropItem(RemovedItemInstance, 1);
}

void UPUBGItemManagerComponent::Server_PickupItemToEquipGunAcc_Implementation(APUBGPickupableItemBase* FromPickupableItem, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType, EGunAccType ToAccType,UPUBGInventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromPickupableItem == nullptr || ToEquipmentManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(ToEquipmentManager) == false || IsAllowedComponent(ToInventoryManager) == false)
	{
		return;
	}

	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	if (ToAccType == EGunAccType::Count)
	{
		return;
	}


	FPUBGPickupInstance PickupInstance = FromPickupableItem->GetPickupInfo();
	UPUBGItemInstance* FromItemInstance = PickupInstance.ItemInstance;
	if (FromItemInstance == nullptr)
	{
		return;
	}
	
	const UPUBGItemFragment_GunAcc* FromWeaponAccFragement = FromItemInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();
	if(FromWeaponAccFragement == nullptr)
	{
		return;
	}

	AActor* Owner = ToInventoryManager->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (ASCInterface == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(UPUBGGA_Interact::StaticClass());
	if (Spec->Ability == nullptr)
	{
		return;
	}

	UPUBGGA_Interact* GA_Interact = Cast<UPUBGGA_Interact>(Spec->Ability);
	if (GA_Interact == nullptr)
	{
		return;
	}

	FVector Center = Owner->GetActorLocation();
	float TraceRange = GA_Interact->GetInteractionTraceRange() + AddtionalRange;
	FBox Box = FromPickupableItem->GetCollisionBoxComponent()->Bounds.GetBox();
	bool bHit = UPUBGPhysicsFunctionLibrary::SphereIntersectsBox(Center, TraceRange, Box);

	if (bHit == false)
	{
		return;
	}

	FPUBGEquipmentEntry& ToEquipmentEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
	UPUBGItemInstance* ToEquipmentItemInstance = ToEquipmentEntry.GetItemInstance();
	APUBGEquipmentBase* ToEquipmentActor = ToEquipmentEntry.SpawnedEquipmentActor;

	UPUBGItemInstance* ToWeaponAccInstance = ToEquipmentActor->GetWeaponAccItemInstance(ToAccType);
	const UPUBGItemFragment_GunAcc* ToWeaponAccFragment = ToWeaponAccInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();
	const UPUBGItemTemplate& ToWeaponTemplate = UPUBGItemData::Get().FindItemTemplateByID(ToWeaponAccInstance->GetItemTemplateID());

	// ������ �׼����� �����
	if (IsValid(ToEquipmentItemInstance))
	{
		UPUBGItemInstance* ToRemovedItemInstance = ToEquipmentActor->UnEquipmentWeaponAcc(ToAccType);

		// ��� �������� -> �κ��丮 ������
		int32 TmpItemCount = 1;
		if (ToInventoryManager->CanAddItem(ToRemovedItemInstance, TmpItemCount))
		{
			int32 ItemCount = 1;
			ToInventoryManager->TryAddItem(ToRemovedItemInstance, ItemCount);
		}
		// �Ⱦ� ���������� ��ȯ
		else
		{
			TryDropItem(ToRemovedItemInstance, 1);
		}
	}

	// �Ⱦ� ���� �ı�
	FromPickupableItem->SetPickupInfo(FPUBGPickupInstance());

	// ��� �׼����� ����
	ToEquipmentActor->EquipmentWeaponAcc(ToAccType, FromItemInstance);
}

void UPUBGItemManagerComponent::Server_PickupItemToEquipment_Implementation(APUBGPickupableItemBase* FromPickupableItem, UPUBGEquipmentManagerComponent* ToEquipmentManager, EEquipmentSlotType ToEquipmentSlotType)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromPickupableItem == nullptr || ToEquipmentManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(ToEquipmentManager) == false)
	{
		return;
	}

	if (ToEquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	FPUBGPickupInstance PickupInstance = FromPickupableItem->GetPickupInfo();
	UPUBGItemInstance* ItemInstance = PickupInstance.ItemInstance;
	if (ItemInstance == nullptr)
	{
		return;
	}

	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
	
	if(EquipableFragment == nullptr)
	{
		return;
	}

	AActor* Owner = ToEquipmentManager->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (ASCInterface == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(UPUBGGA_Interact::StaticClass());
	if (Spec->Ability == nullptr)
	{
		return;
	}

	UPUBGGA_Interact* GA_Interact = Cast<UPUBGGA_Interact>(Spec->Ability);
	if (GA_Interact == nullptr)
	{
		return;
	}

	FVector Center = Owner->GetActorLocation();
	float TraceRange = GA_Interact->GetInteractionTraceRange() + AddtionalRange;
	FBox Box = FromPickupableItem->GetCollisionBoxComponent()->Bounds.GetBox();
	bool bHit = UPUBGPhysicsFunctionLibrary::SphereIntersectsBox(Center, TraceRange, Box);

	if (bHit == false)
	{
		return;
	}

	// �Ⱦ� ������ -> ��� ������
	// ��� ���� ���� Ȯ��
	bool IsEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(EquipableFragment->EquipFeatureConditionClasses, Owner, nullptr, ItemInstance);
	// �Ⱦ� ����
	if (IsEquipable == false)
	{
		return;
	}

	EEquipmentSlotType TargetItemSlotType;
	bool Result = ToEquipmentManager->FindEquipmentSlot(ItemInstance, TargetItemSlotType);
	if (Result == false)
	{
		return;
	}

	UPUBGItemInstance* UnEquipItemInstance = ToEquipmentManager->GetItemInstance(TargetItemSlotType);

	// ���������� ����� �ش� �������� ���� ���������� ���� ��Ų��.
	if (UnEquipItemInstance)
	{
		const UPUBGItemFragment_Equipable* UnEuipableFragment = UnEquipItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
		// ��� ���� ����
		bool UnEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(UnEuipableFragment->UnEquipFeatureConditionClasses, Owner, nullptr, ItemInstance);
		if (UnEquipable == false)
		{
			return;
		}

		// ���� ����
		ToEquipmentManager->Unequipment(TargetItemSlotType);
		// ������ ����
		TryDropItem(UnEquipItemInstance, 1);
	}
	// ���� �������� ������
	else
	{
		// �м� ���� ó��
		// ���� ������ �ƴ����� ������ �����ؾ� �Ѵ�.
		// �м� (�ѹ���)
		if (TargetItemSlotType == EEquipmentSlotType::Whole)
		{
			if (UPUBGItemInstance* ClothTopItemInstance = ToEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothTop))
			{
				// ���� ����
				ToEquipmentManager->Unequipment(EEquipmentSlotType::ClothTop);
				// ������ ����
				TryDropItem(ClothTopItemInstance, 1);
			}

			if (UPUBGItemInstance* ClothBottomItemInstance = ToEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothBottom))
			{
				// ���� ����
				ToEquipmentManager->Unequipment(EEquipmentSlotType::ClothBottom);
				// ������ ����
				TryDropItem(ClothBottomItemInstance, 1);
			}
		}
		// �м� (����)
		else if (TargetItemSlotType == EEquipmentSlotType::ClothTop || TargetItemSlotType == EEquipmentSlotType::ClothBottom)
		{
			if (UPUBGItemInstance* WholeItemInstance = ToEquipmentManager->GetItemInstance(EEquipmentSlotType::Whole))
			{
				// ���� ����
				ToEquipmentManager->Unequipment(EEquipmentSlotType::Whole);
				// ������ ����
				TryDropItem(WholeItemInstance, 1);
			}
		}
	}

	// �Ⱦ� ���� �ı�
	FromPickupableItem->SetPickupInfo(FPUBGPickupInstance());

	// ������ ����
	ToEquipmentManager->Equipment(TargetItemSlotType, ItemInstance);
}

void UPUBGItemManagerComponent::Server_PickupItemToInventory_Implementation(APUBGPickupableItemBase* FromPickupableItem, UPUBGInventoryManagerComponent* ToInventoryManager)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (FromPickupableItem == nullptr || ToInventoryManager == nullptr)
	{
		return;
	}

	if (IsAllowedComponent(ToInventoryManager) == false)
	{
		return;
	}

	FPUBGPickupInstance PickupInstance = FromPickupableItem->GetPickupInfo();
	UPUBGItemInstance* ItemInstance = PickupInstance.ItemInstance;
	int32 ItemCount = PickupInstance.ItemCount;
	if (ItemInstance == nullptr)
	{
		return;
	}

	AActor* Owner = ToInventoryManager->GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (ASCInterface == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(UPUBGGA_Interact::StaticClass());
	if (Spec->Ability == nullptr)
	{
		return;
	}

	UPUBGGA_Interact* GA_Interact = Cast<UPUBGGA_Interact>(Spec->Ability);
	if (GA_Interact == nullptr)
	{
		return;
	}

	FVector Center = Owner->GetActorLocation();
	float TraceRange = GA_Interact->GetInteractionTraceRange() + AddtionalRange;
	FBox Box = FromPickupableItem->GetCollisionBoxComponent()->Bounds.GetBox();
	bool bHit = UPUBGPhysicsFunctionLibrary::SphereIntersectsBox(Center, TraceRange, Box);

	if (bHit == false)
	{
		return;
	}

	if (ToInventoryManager->TryAddItem(ItemInstance, ItemCount))
	{
		// �Ⱦ� ���� �ı�
		FromPickupableItem->SetPickupInfo(FPUBGPickupInstance());
	}
	else
	{
		PickupInstance.ItemCount = ItemCount;
		FromPickupableItem->SetPickupInfo(PickupInstance);
	}
}

bool UPUBGItemManagerComponent::TryPickupItem(APUBGPickupableItemBase* PickupableItem)
{
	if (HasAuthority() == false)
	{
		return false;
	}

	if (IsValid(PickupableItem) == false)
	{
		return false;
	}

	if (IsValid(PickupableItem->GetPickupInfo().ItemInstance) == false)
	{
		return false;
	}

	AController* Controller = Cast<AController>(GetOwner());
	APawn* Pawn = Controller ? Cast<APawn>(Controller->GetPawn()) : Cast<APawn>(GetOwner());

	if (Pawn == nullptr)
	{
		return false;
	}

	UPUBGEquipmentManagerComponent* PawnEquipmentManager = GetPawnEquipmentManager();
	UPUBGInventoryManagerComponent* PawnInventoryManager = GetPawnInventoryManager();
	if (PawnEquipmentManager == nullptr)
	{
		return false;
	}

	// ��� ���� ������������ ����
	bool IsEquipImmediately = false;
	FPUBGPickupInstance PickupInstance = PickupableItem->GetPickupInfo();
	UPUBGItemInstance* ItemInstance = PickupInstance.ItemInstance;
	int32 ItemCount = PickupInstance.ItemCount;
	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();

	if (EquipableFragment)
	{
		IsEquipImmediately = EquipableFragment->EquipmentImmediately;
	}

	// ��� �����̸� ��� ���� �õ�
	if (IsEquipImmediately)
	{
		// ��� ���� ���� Ȯ��
		bool IsEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(EquipableFragment->EquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
		// �Ⱦ� ����
		if (IsEquipable == false)
		{
			return false;
		}


		EEquipmentSlotType TargetItemSlotType;
		bool Result = PawnEquipmentManager->FindEquipmentSlot(ItemInstance, TargetItemSlotType);
		if (Result == false)
		{
			return false;
		}

		UPUBGItemInstance* UnEquipItemInstance = PawnEquipmentManager->GetItemInstance(TargetItemSlotType);

		// ���������� ����� �ش� �������� ���� ���������� ���� ��Ų��.
		if (UnEquipItemInstance)
		{
			const UPUBGItemFragment_Equipable* UnEuipableFragment = UnEquipItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
			// ��� ���� ����
			bool UnEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(UnEuipableFragment->UnEquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
			if (UnEquipable == false)
			{
				return false;
			}

			// ���� ����
			PawnEquipmentManager->Unequipment(TargetItemSlotType);
			// ������ ����
			TryDropItem(UnEquipItemInstance, 1);
		}
		// ���� �������� ������
		else
		{
			// �м� ���� ó��
			// ���� ������ �ƴ����� ������ �����ؾ� �Ѵ�.
			// �м� (�ѹ���)
			if (TargetItemSlotType == EEquipmentSlotType::Whole)
			{
				if (UPUBGItemInstance* ClothTopItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothTop))
				{
					// ���� ����
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothTop);
					// ������ ����
					TryDropItem(ClothTopItemInstance, 1);
				}

				if (UPUBGItemInstance* ClothBottomItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothBottom))
				{
					// ���� ����
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothBottom);
					// ������ ����
					TryDropItem(ClothBottomItemInstance, 1);
				}
			}
			// �м� (����)
			else if (TargetItemSlotType == EEquipmentSlotType::ClothTop || TargetItemSlotType == EEquipmentSlotType::ClothBottom)
			{
				if (UPUBGItemInstance* WholeItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::Whole))
				{
					// ���� ����
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::Whole);
					// ������ ����
					TryDropItem(WholeItemInstance, 1);
				}
			}
		}

		// �Ⱦ� ���� �ı�
		PickupableItem->SetPickupInfo(FPUBGPickupInstance());

		// ������ ����
		PawnEquipmentManager->Equipment(TargetItemSlotType, ItemInstance);

		return true;
	}
	// �κ��丮��
	else
	{
		if (PawnInventoryManager->TryAddItem(ItemInstance, ItemCount))
		{
			// �Ⱦ� ���� �ı�
			PickupableItem->SetPickupInfo(FPUBGPickupInstance());
			return true;
		}
		else
		{
			PickupInstance.ItemCount = ItemCount;
			PickupableItem->SetPickupInfo(PickupInstance);
			return false;
		}
	}
}

bool UPUBGItemManagerComponent::TryAddItem(UPUBGItemInstance* InItemInstance, int32 InItemCount)
{
	if (HasAuthority() == false)
	{
		return false;
	}

	if (IsValid(InItemInstance) == false)
	{
		return false;
	}

	if (InItemCount == 0)
	{
		return false;
	}

	AController* Controller = Cast<AController>(GetOwner());
	APawn* Pawn = Controller ? Cast<APawn>(Controller->GetPawn()) : Cast<APawn>(GetOwner());

	if (Pawn == nullptr)
	{
		return false;
	}

	UPUBGEquipmentManagerComponent* PawnEquipmentManager = GetPawnEquipmentManager();
	UPUBGInventoryManagerComponent* PawnInventoryManager = GetPawnInventoryManager();
	if (PawnEquipmentManager == nullptr)
	{
		return false;
	}

	// ��� ���� ������������ ����
	bool IsEquipImmediately = false;
	UPUBGItemInstance* ItemInstance = InItemInstance;
	int32 ItemCount = InItemCount;
	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();

	if (EquipableFragment)
	{
		IsEquipImmediately = EquipableFragment->EquipmentImmediately;
	}

	// ��� �����̸� ��� ���� �õ�
	if (IsEquipImmediately)
	{
		// ��� ���� ���� Ȯ��
		bool IsEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(EquipableFragment->EquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
		// �Ⱦ� ����
		if (IsEquipable == false)
		{
			return false;
		}


		EEquipmentSlotType TargetItemSlotType;
		bool Result = PawnEquipmentManager->FindEquipmentSlot(ItemInstance, TargetItemSlotType);
		if (Result == false)
		{
			return false;
		}

		UPUBGItemInstance* UnEquipItemInstance = PawnEquipmentManager->GetItemInstance(TargetItemSlotType);

		// ���������� ����� �ش� �������� ���� ���������� ���� ��Ų��.
		if (UnEquipItemInstance)
		{
			const UPUBGItemFragment_Equipable* UnEuipableFragment = UnEquipItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
			// ��� ���� ����
			bool UnEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(UnEuipableFragment->UnEquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
			if (UnEquipable == false)
			{
				return false;
			}

			// ���� ����
			PawnEquipmentManager->Unequipment(TargetItemSlotType);
			// ������ ����
			TryDropItem(UnEquipItemInstance, 1);
		}
		// ���� �������� ������
		else
		{
			// �м� ���� ó��
			// ���� ������ �ƴ����� ������ �����ؾ� �Ѵ�.
			// �м� (�ѹ���)
			if (TargetItemSlotType == EEquipmentSlotType::Whole)
			{
				if (UPUBGItemInstance* ClothTopItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothTop))
				{
					// ���� ����
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothTop);
					// ������ ����
					TryDropItem(ClothTopItemInstance, 1);
				}

				if (UPUBGItemInstance* ClothBottomItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothBottom))
				{
					// ���� ����
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothBottom);
					// ������ ����
					TryDropItem(ClothBottomItemInstance, 1);
				}
			}
			// �м� (����)
			else if (TargetItemSlotType == EEquipmentSlotType::ClothTop || TargetItemSlotType == EEquipmentSlotType::ClothBottom)
			{
				if (UPUBGItemInstance* WholeItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::Whole))
				{
					// ���� ����
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::Whole);
					// ������ ����
					TryDropItem(WholeItemInstance, 1);
				}
			}
		}

		// ������ ����
		PawnEquipmentManager->Equipment(TargetItemSlotType, ItemInstance);

		return true;
	}
	// �κ��丮��
	else
	{
		if (PawnInventoryManager->TryAddItem(ItemInstance, ItemCount))
		{
			// �Ⱦ� ���� �ı�
			return true;
		}
		else
		{
			TryDropItem(InItemInstance, ItemCount);
			return false;
		}
	}
}

// �ν��Ͻ� ��ü�� ���Ϳ� �־������ �������� ������ ���ο� ������ �ν��Ͻ��� �����ؾ��Ѵ�.
bool UPUBGItemManagerComponent::TryDropItem(UPUBGItemInstance* ItemInstance, int32 ItemCount)
{
	if (HasAuthority() == false)
	{
		return false;
	}

	if (ItemCount == 0)
	{
		return false;
	}

	APUBGPickupableItemBase* PickupableItemActor = SpawnPickupableItem(ItemInstance, ItemCount);

	return (PickupableItemActor != nullptr);
}


APUBGPickupableItemBase* UPUBGItemManagerComponent::SpawnPickupableItem(UPUBGItemInstance* ItemInstance, int32 Count) const
{
	if (HasAuthority() == false)
	{
		return nullptr;
	}

	if (IsValid(ItemInstance) == false)
	{
		return nullptr;
	}

	AController* Controller = Cast<AController>(GetOwner());
	APawn* Pawn = Controller ? Cast<APawn>(Controller->GetPawn()) : Cast<APawn>(GetOwner());

	if (Pawn == nullptr)
	{
		return nullptr;
	}

	// ������ ����
	FVector TraceStartLocation = Pawn->GetActorLocation();
	FVector TraceEndLocation = TraceStartLocation + FVector(0.0f, 0.0f, -100000.0f);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Pawn);
	Pawn->GetAttachedActors(IgnoreActors, false, true);

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(IgnoreActors);

	FHitResult HitResult;
	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECollisionChannel::ECC_Visibility, Params);

	FVector SpawnLocation = IsHit ? HitResult.Location : TraceStartLocation;

	TSubclassOf<APUBGPickupableItemBase> PickupableItemBaseClass = UPUBGAssetManager::GetSubclassByName<APUBGPickupableItemBase>("PickupableItemBaseClass");
	APUBGPickupableItemBase* NewPickupableItemActor = GetWorld()->SpawnActor<APUBGPickupableItemBase>(PickupableItemBaseClass, SpawnLocation, FRotator::ZeroRotator);

	FPUBGPickupInstance PickupInstance;
	PickupInstance.ItemInstance = ItemInstance;
	PickupInstance.ItemCount = Count;
	if (NewPickupableItemActor->IsUsingRegisteredSubObjectList())
	{
		// 1ȸ ȣ�� �ѹ��� ����
		NewPickupableItemActor->AddReplicatedSubObject(ItemInstance);
	}

	NewPickupableItemActor->SetPickupInfo(PickupInstance);

	return NewPickupableItemActor;
}

void UPUBGItemManagerComponent::AddAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Add(ActorComponent);
}

void UPUBGItemManagerComponent::RemoveAllowedComponent(UActorComponent* ActorComponent)
{
	AllowedComponents.Remove(ActorComponent);
}

bool UPUBGItemManagerComponent::IsAllowedComponent(UActorComponent* ActorComponent)
{
	return AllowedComponents.Contains(ActorComponent);
}

UPUBGEquipmentManagerComponent* UPUBGItemManagerComponent::GetPawnEquipmentManager() const
{
	UPUBGEquipmentManagerComponent* PawnEquipmentManager = nullptr;
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			PawnEquipmentManager = Pawn->GetComponentByClass<UPUBGEquipmentManagerComponent>();
		}
	}

	return PawnEquipmentManager;
}

UPUBGInventoryManagerComponent* UPUBGItemManagerComponent::GetPawnInventoryManager() const
{
	UPUBGInventoryManagerComponent* PawnInventoryManager = nullptr;
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			PawnInventoryManager = Pawn->GetComponentByClass<UPUBGInventoryManagerComponent>();
		}
	}

	return PawnInventoryManager;
}