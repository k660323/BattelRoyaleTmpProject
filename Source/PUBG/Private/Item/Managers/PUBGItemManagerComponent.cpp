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
		// ex) 인벤토리 수류탄 -> 어느 투척 무기 (스왑)
		//	   인벤토리 수류탄 -> 빈 장비
		
		FPUBGEquipmentEntry& ToEquipmentEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
		UPUBGItemInstance* ToItemInstance = ToEquipmentEntry.GetItemInstance();
		// 장비 존재시
		if (IsValid(ToItemInstance))
		{
			if (FromItemInstance->GetItemTemplateID() == ToItemInstance->GetItemTemplateID())
			{
				return;
			}

			// 인벤토리에서도 하나 제거
			UPUBGItemInstance* RemovedItemInstanceFrom = FromInventoryManager->TryRemoveItem(FromItemIndex, 1);
			// 현재 장비를 제거
			UPUBGItemInstance* RemovedItemInstanceTo = ToEquipmentManager->Unequipment(ToEquipmentSlotType);

			// 인벤토리 아이템을 -> 장비로
			ToEquipmentManager->Equipment(ToEquipmentSlotType, RemovedItemInstanceFrom);

			// 장비 아이템을 -> 인벤토리 아이템
			int32 TmpItemCount = 1;
			if (FromInventoryManager->CanAddItem(RemovedItemInstanceTo, TmpItemCount))
			{
				int32 ItemCount = 1;
				FromInventoryManager->TryAddItem(RemovedItemInstanceTo, ItemCount);
			}
			// 픽업 아이템으로 변환
			else
			{
				TryDropItem(RemovedItemInstanceTo, 1);
			}
		}
		// 장비 미존재시
		else
		{
			//int32 FromItemCount = FromInventoryManager->GetItemCount(FromItemIndex);
			// 아이템 장착 (아이템 하나빼서 장착)
			int32 MovableCount = FromInventoryManager->CanRemoveItem(FromItemIndex, 1);
			if (MovableCount > 0)
			{
				// 인벤에서 아이템 하나가져오기
				UPUBGItemInstance* RemovedItemInstance = FromInventoryManager->TryRemoveItem(FromItemIndex, 1);
				// 장비창에 넣기
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


	// TODO : 장착 가능한지 확인 함수

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

	// 장비 액세서리 존재시
	if (ToEquipmentActor->IsValidWeaponAcc(ToAccType))
	{
		UPUBGItemInstance* RemovedItemInstance = ToEquipmentActor->UnEquipmentWeaponAcc(ToAccType);
		// 장비 아이템을 -> 인벤토리 아이템
		int32 TmpItemCount = 1;
		if (FromInventoryManager->CanAddItem(RemovedItemInstance, TmpItemCount))
		{
			int32 ItemCount = 1;
			FromInventoryManager->TryAddItem(RemovedItemInstance, ItemCount);
		}
		// 픽업 아이템으로 변환
		else
		{
			TryDropItem(RemovedItemInstance, 1);
		}
	}

	// 장비 액세서리 장착
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

	// 소켓이 한쪽만 열려있으면 스킵
	if (bFromOpenSocket == false || bToOpenSocket == false)
	{
		return;
	}
	
	UPUBGItemInstance* FromWeaponAccInstance = FromEquipmentActor->GetWeaponAccItemInstance(FromAccType);
	const UPUBGItemFragment_GunAcc* FromWeaponAccFragment = FromWeaponAccInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();
	const UPUBGItemTemplate& FromWeaponTemplate = UPUBGItemData::Get().FindItemTemplateByID(FromWeaponAccInstance->GetItemTemplateID());

	// 시작점의 아이템인스턴스가 없으면 스킵
	if (IsValid(FromWeaponAccInstance) == false)
	{
		return;
	}

	UPUBGItemInstance* ToWeaponAccInstance = ToEquipmentActor->GetWeaponAccItemInstance(ToAccType);
	const UPUBGItemFragment_GunAcc* ToWeaponAccFragment = ToWeaponAccInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();
	const UPUBGItemTemplate& ToWeaponTemplate = UPUBGItemData::Get().FindItemTemplateByID(ToWeaponAccInstance->GetItemTemplateID());
	
	bool bToEquipWeaponAccByFrom = FromWeaponAccFragment->GunAccMeshMap.Contains(ToWeaponTemplate.GetClass());
	bool bFromEquipWeaponAccByTo = ToWeaponAccFragment->GunAccMeshMap.Contains(FromWeaponTemplate.GetClass());

	// 목표지점에 아이템이 존재할 경우
	if (IsValid(ToWeaponAccInstance))
	{
		// 파츠 호환이 되는경우 스왑
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

			// 인벤 또는 바닥으로 (To)
			// 장비 액세서리 존재시
			UPUBGItemInstance* RemovedItemInstance = ToWeaponAccInstance;
			// 장비 아이템을 -> 인벤토리 아이템
			int32 TmpItemCount = 1;
			if (ToInventoryManager->CanAddItem(RemovedItemInstance, TmpItemCount))
			{
				int32 ItemCount = 1;
				ToInventoryManager->TryAddItem(RemovedItemInstance, ItemCount);
			}
			// 픽업 아이템으로 변환
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

	// 주무기 교체
	if ((FromEquipmentSlotType == EEquipmentSlotType::Main && ToEquipmentSlotType == EEquipmentSlotType::Main2) ||
		(FromEquipmentSlotType == EEquipmentSlotType::Main2 && ToEquipmentSlotType == EEquipmentSlotType::Main))
	{
		FPUBGEquipmentEntry& FromEntry =  FromEquipmentManager->GetEntry(FromEquipmentSlotType);
		UPUBGItemInstance* FromItemInstance = FromEntry.GetItemInstance();
		FPUBGEquipmentEntry& ToEntry = ToEquipmentManager->GetEntry(ToEquipmentSlotType);
		UPUBGItemInstance* ToItemInstance = ToEntry.GetItemInstance();

		// 두무기 존재
		if (FromItemInstance && ToItemInstance)
		{
			// Equipment 함수가 기존 아이템이 있으면 제거하고 장착한다.
			FromEquipmentManager->Equipment(ToEquipmentSlotType, ToItemInstance);
			ToEquipmentManager->Equipment(FromEquipmentSlotType, FromItemInstance);
		}
		// 한쪽만 존재
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

	// 무기 파츠 제거
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

	// 버릴수 있는 카운터가 존재
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

	// 기존의 액세서리 존재시
	if (IsValid(ToEquipmentItemInstance))
	{
		UPUBGItemInstance* ToRemovedItemInstance = ToEquipmentActor->UnEquipmentWeaponAcc(ToAccType);

		// 장비 아이템을 -> 인벤토리 아이템
		int32 TmpItemCount = 1;
		if (ToInventoryManager->CanAddItem(ToRemovedItemInstance, TmpItemCount))
		{
			int32 ItemCount = 1;
			ToInventoryManager->TryAddItem(ToRemovedItemInstance, ItemCount);
		}
		// 픽업 아이템으로 변환
		else
		{
			TryDropItem(ToRemovedItemInstance, 1);
		}
	}

	// 픽업 액터 파괴
	FromPickupableItem->SetPickupInfo(FPUBGPickupInstance());

	// 장비 액세서리 장착
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

	// 픽업 아이템 -> 장비 아이템
	// 장비 장착 조건 확인
	bool IsEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(EquipableFragment->EquipFeatureConditionClasses, Owner, nullptr, ItemInstance);
	// 픽업 실패
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

	// 장착아이템 존재시 해당 아이템을 월드 아이템으로 스폰 시킨다.
	if (UnEquipItemInstance)
	{
		const UPUBGItemFragment_Equipable* UnEuipableFragment = UnEquipItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
		// 장비 해제 실패
		bool UnEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(UnEuipableFragment->UnEquipFeatureConditionClasses, Owner, nullptr, ItemInstance);
		if (UnEquipable == false)
		{
			return;
		}

		// 장착 해제
		ToEquipmentManager->Unequipment(TargetItemSlotType);
		// 아이템 스폰
		TryDropItem(UnEquipItemInstance, 1);
	}
	// 장착 아이템이 없으면
	else
	{
		// 패션 예외 처리
		// 같은 부위가 아니지만 장착을 해제해야 한다.
		// 패션 (한벌옷)
		if (TargetItemSlotType == EEquipmentSlotType::Whole)
		{
			if (UPUBGItemInstance* ClothTopItemInstance = ToEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothTop))
			{
				// 장착 해제
				ToEquipmentManager->Unequipment(EEquipmentSlotType::ClothTop);
				// 아이템 스폰
				TryDropItem(ClothTopItemInstance, 1);
			}

			if (UPUBGItemInstance* ClothBottomItemInstance = ToEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothBottom))
			{
				// 장착 해제
				ToEquipmentManager->Unequipment(EEquipmentSlotType::ClothBottom);
				// 아이템 스폰
				TryDropItem(ClothBottomItemInstance, 1);
			}
		}
		// 패션 (상의)
		else if (TargetItemSlotType == EEquipmentSlotType::ClothTop || TargetItemSlotType == EEquipmentSlotType::ClothBottom)
		{
			if (UPUBGItemInstance* WholeItemInstance = ToEquipmentManager->GetItemInstance(EEquipmentSlotType::Whole))
			{
				// 장착 해제
				ToEquipmentManager->Unequipment(EEquipmentSlotType::Whole);
				// 아이템 스폰
				TryDropItem(WholeItemInstance, 1);
			}
		}
	}

	// 픽업 액터 파괴
	FromPickupableItem->SetPickupInfo(FPUBGPickupInstance());

	// 아이템 장착
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
		// 픽업 액터 파괴
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

	// 즉시 장착 아이템인지만 구분
	bool IsEquipImmediately = false;
	FPUBGPickupInstance PickupInstance = PickupableItem->GetPickupInfo();
	UPUBGItemInstance* ItemInstance = PickupInstance.ItemInstance;
	int32 ItemCount = PickupInstance.ItemCount;
	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();

	if (EquipableFragment)
	{
		IsEquipImmediately = EquipableFragment->EquipmentImmediately;
	}

	// 즉시 장착이면 장비 장착 시도
	if (IsEquipImmediately)
	{
		// 장비 장착 조건 확인
		bool IsEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(EquipableFragment->EquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
		// 픽업 실패
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

		// 장착아이템 존재시 해당 아이템을 월드 아이템으로 스폰 시킨다.
		if (UnEquipItemInstance)
		{
			const UPUBGItemFragment_Equipable* UnEuipableFragment = UnEquipItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
			// 장비 해제 실패
			bool UnEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(UnEuipableFragment->UnEquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
			if (UnEquipable == false)
			{
				return false;
			}

			// 장착 해제
			PawnEquipmentManager->Unequipment(TargetItemSlotType);
			// 아이템 스폰
			TryDropItem(UnEquipItemInstance, 1);
		}
		// 장착 아이템이 없으면
		else
		{
			// 패션 예외 처리
			// 같은 부위가 아니지만 장착을 해제해야 한다.
			// 패션 (한벌옷)
			if (TargetItemSlotType == EEquipmentSlotType::Whole)
			{
				if (UPUBGItemInstance* ClothTopItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothTop))
				{
					// 장착 해제
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothTop);
					// 아이템 스폰
					TryDropItem(ClothTopItemInstance, 1);
				}

				if (UPUBGItemInstance* ClothBottomItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothBottom))
				{
					// 장착 해제
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothBottom);
					// 아이템 스폰
					TryDropItem(ClothBottomItemInstance, 1);
				}
			}
			// 패션 (상의)
			else if (TargetItemSlotType == EEquipmentSlotType::ClothTop || TargetItemSlotType == EEquipmentSlotType::ClothBottom)
			{
				if (UPUBGItemInstance* WholeItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::Whole))
				{
					// 장착 해제
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::Whole);
					// 아이템 스폰
					TryDropItem(WholeItemInstance, 1);
				}
			}
		}

		// 픽업 액터 파괴
		PickupableItem->SetPickupInfo(FPUBGPickupInstance());

		// 아이템 장착
		PawnEquipmentManager->Equipment(TargetItemSlotType, ItemInstance);

		return true;
	}
	// 인벤토리행
	else
	{
		if (PawnInventoryManager->TryAddItem(ItemInstance, ItemCount))
		{
			// 픽업 액터 파괴
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

	// 즉시 장착 아이템인지만 구분
	bool IsEquipImmediately = false;
	UPUBGItemInstance* ItemInstance = InItemInstance;
	int32 ItemCount = InItemCount;
	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();

	if (EquipableFragment)
	{
		IsEquipImmediately = EquipableFragment->EquipmentImmediately;
	}

	// 즉시 장착이면 장비 장착 시도
	if (IsEquipImmediately)
	{
		// 장비 장착 조건 확인
		bool IsEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(EquipableFragment->EquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
		// 픽업 실패
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

		// 장착아이템 존재시 해당 아이템을 월드 아이템으로 스폰 시킨다.
		if (UnEquipItemInstance)
		{
			const UPUBGItemFragment_Equipable* UnEuipableFragment = UnEquipItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
			// 장비 해제 실패
			bool UnEquipable = UPUBGFeatureEquipmentCondition::ExecuteEquipmentFeatureConditions(UnEuipableFragment->UnEquipFeatureConditionClasses, Pawn, nullptr, ItemInstance);
			if (UnEquipable == false)
			{
				return false;
			}

			// 장착 해제
			PawnEquipmentManager->Unequipment(TargetItemSlotType);
			// 아이템 스폰
			TryDropItem(UnEquipItemInstance, 1);
		}
		// 장착 아이템이 없으면
		else
		{
			// 패션 예외 처리
			// 같은 부위가 아니지만 장착을 해제해야 한다.
			// 패션 (한벌옷)
			if (TargetItemSlotType == EEquipmentSlotType::Whole)
			{
				if (UPUBGItemInstance* ClothTopItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothTop))
				{
					// 장착 해제
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothTop);
					// 아이템 스폰
					TryDropItem(ClothTopItemInstance, 1);
				}

				if (UPUBGItemInstance* ClothBottomItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::ClothBottom))
				{
					// 장착 해제
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::ClothBottom);
					// 아이템 스폰
					TryDropItem(ClothBottomItemInstance, 1);
				}
			}
			// 패션 (상의)
			else if (TargetItemSlotType == EEquipmentSlotType::ClothTop || TargetItemSlotType == EEquipmentSlotType::ClothBottom)
			{
				if (UPUBGItemInstance* WholeItemInstance = PawnEquipmentManager->GetItemInstance(EEquipmentSlotType::Whole))
				{
					// 장착 해제
					PawnEquipmentManager->Unequipment(EEquipmentSlotType::Whole);
					// 아이템 스폰
					TryDropItem(WholeItemInstance, 1);
				}
			}
		}

		// 아이템 장착
		PawnEquipmentManager->Equipment(TargetItemSlotType, ItemInstance);

		return true;
	}
	// 인벤토리행
	else
	{
		if (PawnInventoryManager->TryAddItem(ItemInstance, ItemCount))
		{
			// 픽업 액터 파괴
			return true;
		}
		else
		{
			TryDropItem(InItemInstance, ItemCount);
			return false;
		}
	}
}

// 인스턴스 자체를 액터에 넣어버려서 아이템을 버릴시 새로운 아이템 인스턴스를 생성해야한다.
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

	// 아이템 스폰
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
		// 1회 호출 한번만 복제
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