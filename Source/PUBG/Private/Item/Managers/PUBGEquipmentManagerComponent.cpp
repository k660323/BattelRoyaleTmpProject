// Kakanu Portfolio Project


#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Controller/PUBGPlayerController.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/PUBGItemInstance.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "Item/Managers/PUBGCosmeticManagerComponent.h"
#include "Actors/PUBGEquipmentBase.h"
#include "System/PUBGAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGEquipmentManagerComponent)

// FPUBGEquipmentEntry Begin ---------------------------------------------------------------------------------------
void FPUBGEquipmentEntry::Init(UPUBGItemInstance* InItemInstance)
{
	check(InItemInstance);


	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = EquipmentManager->GetActiveEquipmentManager();
	if (ActiveEquipmentManager == nullptr)
	{
		return;
	}

	// ���� ItemInstance �� ��� ó���� �Ǵ°�?
	if (ItemInstance)
	{
		// ���־� ���� (����, ���� ������)
		Unequipment();
		ActiveEquipmentManager->Unequip(EquipmentSlotType);
	}

	ItemInstance = InItemInstance;

	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
	if (EquipableFragment == nullptr)
	{
		return;

	}

	// ����� ���� �׼������� ������ �������� ��� ��� ��� (TODO : ������� ����� ���� ������ ��쵵 ��� ����)
	if (EquipableFragment->EquipmentType == EEquipmentType::Armor || EquipableFragment->EquipmentType == EEquipmentType::Fashion ||
		EquipableFragment->EquipmentType == EEquipmentType::Utility)
	{
		ActiveEquipmentManager->Equip(EquipmentSlotType, ItemInstance);
	}

	// ���־� ���� (����, ���� ������)
	if (InItemInstance)
	{
		Equipment();
	}
}

UPUBGItemInstance* FPUBGEquipmentEntry::Reset()
{
	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = EquipmentManager->GetActiveEquipmentManager();
	if (ActiveEquipmentManager == nullptr)
	{
		return nullptr;
	}

	// ��� ������ ���� ����
	if (ItemInstance)
	{
		ActiveEquipmentManager->Unequip(EquipmentSlotType);
		// ���־� ���� ����
		Unequipment();
	}

	UPUBGItemInstance* RemoveItemInstance = ItemInstance;
	ItemInstance = nullptr;

	// ���� ������ ������ �ν��Ͻ��� nullptr�̸� �Ǽ� ����
	if (EquipmentManager->IsSocketEmpty(ActiveEquipmentManager->GetCurActiveWeaponEquipmentType()))
	{
		ActiveEquipmentManager->ChangeEquipState(EActiveWeaponEquipmentType::Unarmed);
	}

	return RemoveItemInstance;
}

// Ŭ��/���� �Ѵ� ����
void FPUBGEquipmentEntry::Equipment()
{
	APUBGBaseCharacter* Character = EquipmentManager->GetCharacter();

	if (Character == nullptr)
	{
		return;
	}


	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = EquipmentManager->GetActiveEquipmentManager();
	if (ActiveEquipmentManager == nullptr)
	{
		return;
	}

	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
	if (EquipableFragment == nullptr)
	{
		return;
	}

	if (EquipmentManager->GetOwner()->HasAuthority())
	{
		//// ���� ���� ����
		if (EquipableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			// Despawn Previous Real Weapon
			if (IsValid(SpawnedEquipmentActor))
			{
				SpawnedEquipmentActor->Destroy();
			}

			const UPUBGItemFragment_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();
			const FPUBGWeaponAttachInfo& AttachInfo = AttachmentFragment->AttachInfo;

			if (AttachInfo.SpawnWeaponClass)
			{
				USkeletalMeshComponent* MeshComponent = Character->GetMesh();
				UWorld* World = ActiveEquipmentManager->GetWorld();
				APUBGEquipmentBase* NewWeaponActor = World->SpawnActorDeferred<APUBGEquipmentBase>(AttachInfo.SpawnWeaponClass, FTransform::Identity, Character);
				NewWeaponActor->Init(ItemInstance->GetItemTemplateID(), EquipmentSlotType);
				
				FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
			
				// Ȱ��ȭ�� ���Ⱑ ������
				EActiveWeaponEquipmentType ActiveEquipmentState = EquipmentManager->GetActiveEquipmentManager()->GetCurActiveWeaponEquipmentType();
				if (ActiveEquipmentState == EActiveWeaponEquipmentType::Unarmed || ActiveEquipmentState == EActiveWeaponEquipmentType::Count)
				{
					UPUBGAbilitySystemComponent* ASC = EquipmentManager->GetPUBGAbilitySystemComponent();
					FName AttachSocket = AttachmentFragment->GetActiveEquipmentAttachSocket(ASC);
					FTransform NewTransform = MeshComponent->GetSocketTransform(AttachSocket);
					NewWeaponActor->SetActorTransform(NewTransform);
					NewWeaponActor->AttachToComponent(MeshComponent, AttachmentTransformRules, AttachSocket);
					NewWeaponActor->SetActorRelativeTransform(AttachInfo.AttachTransformOffset);
				}
				// Ȱ��ȭ�� ���Ⱑ �����
				// ���Ͽ� �°� ���� ����
				else
				{
					FName AttachSocket = EquipmentManager->GetWeaponEquipmentAttachSocket(EquipmentSlotType);
					FTransform NewTransform = MeshComponent->GetSocketTransform(AttachSocket);
					NewWeaponActor->SetActorTransform(NewTransform);
					NewWeaponActor->AttachToComponent(MeshComponent, AttachmentTransformRules, AttachSocket);
				}

				NewWeaponActor->FinishSpawning(FTransform::Identity, true);
			}
		}
	}
	else
	{
		// �ش� ƽ���� �������� ����� �����Ͱ� ����
		if (EquipableFragment->EquipmentType == EEquipmentType::Weapon)
		{
			// Despawn Previous Real Weapon
			if (IsValid(SpawnedEquipmentActor))
			{
				SpawnedEquipmentActor->Destroy();
			}
		}
		else if (EquipableFragment->EquipmentType == EEquipmentType::Fashion)
		{
			const UPUBGItemFragment_Fashion* FashionFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Fashion>();

			if (UPUBGCosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UPUBGCosmeticManagerComponent>())
			{
				USkeletalMesh* LoadedSkeletalMesh = UPUBGAssetManager::GetAssetByPath(FashionFragment->FashionMesh);
				UTexture* LoadedMaskTexture = UPUBGAssetManager::GetAssetByPath(FashionFragment->MaskTexture);
				CharacterCosmetics->ReplaceSkeletalMesh(FashionFragment->FashionType, LoadedSkeletalMesh, LoadedMaskTexture);
			}
		}
		else if (EquipableFragment->EquipmentType == EEquipmentType::Armor)
		{
			const UPUBGItemFragment_Armor* ArmorFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Armor>();
			if (UPUBGCosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UPUBGCosmeticManagerComponent>())
			{
				UStaticMesh* LoadedStaticMesh = UPUBGAssetManager::GetAssetByPath(ArmorFragment->ArmorMesh);
				CharacterCosmetics->ReplaceArmorMesh(ArmorFragment->ArmorType, LoadedStaticMesh, ArmorFragment->AttachSocket);
			}
		}
		else if (EquipableFragment->EquipmentType == EEquipmentType::Utility)
		{
			const UPUBGItemFragment_Utility* UtilityFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Utility>();
			if (UPUBGCosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UPUBGCosmeticManagerComponent>())
			{
				UStaticMesh* LoadedStaticMesh = UPUBGAssetManager::GetAssetByPath(UtilityFragment->UtilityMesh);
				CharacterCosmetics->ReplaceUtilityMesh(UtilityFragment->UtilityType, LoadedStaticMesh, UtilityFragment->AttachSocket);
			}
		}
	}
}

// Ŭ��/���� �Ѵ� ����
void FPUBGEquipmentEntry::Unequipment()
{
	APUBGBaseCharacter* Character = EquipmentManager->GetCharacter();

	if (Character == nullptr)
	{
		return;
	}

	if (EquipmentManager->GetOwner()->HasAuthority())
	{
		if (UPUBGEquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
		{
			// Despawn Previous Real Weapon
			if (IsValid(SpawnedEquipmentActor))
			{
				SpawnedEquipmentActor->Destroy();
			}
		}
	}
	else
	{
		// �ش� ƽ���� �������� ����� �����Ͱ� ����
		if (UPUBGEquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
		{
			// Despawn Previous Real Weapon
			if (IsValid(SpawnedEquipmentActor))
			{
				SpawnedEquipmentActor->Destroy();
			}
		}
		else if (UPUBGEquipmentManagerComponent::IsFashionSlot(EquipmentSlotType))
		{
			EFashionType RemovedFashionType = UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToFashionType(EquipmentSlotType);
			if (RemovedFashionType != EFashionType::Count)
			{
				if (UPUBGCosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UPUBGCosmeticManagerComponent>())
				{
					CharacterCosmetics->ReplaceSkeletalMesh(RemovedFashionType, nullptr, nullptr);
				}
			}
		}
		else if (UPUBGEquipmentManagerComponent::IsArmorSlot(EquipmentSlotType))
		{
			EArmorType RemovedArmorType = UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToArmorType(EquipmentSlotType);
			if (RemovedArmorType != EArmorType::Count)
			{
				if (UPUBGCosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UPUBGCosmeticManagerComponent>())
				{
					CharacterCosmetics->ReplaceArmorMesh(RemovedArmorType, nullptr, FName());
				}
			}
		}
		else if (UPUBGEquipmentManagerComponent::IsUtilitySlot(EquipmentSlotType))
		{
			EUtilityType RemovedUtilityType = UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToUtilityType(EquipmentSlotType);
			if (RemovedUtilityType != EUtilityType::Count)
			{
				if (UPUBGCosmeticManagerComponent* CharacterCosmetics = Character->FindComponentByClass<UPUBGCosmeticManagerComponent>())
				{
					CharacterCosmetics->ReplaceUtilityMesh(RemovedUtilityType, nullptr, FName());
				}
			}
		}
	}
}

// FPUBGEquipmentEntry End ---------------------------------------------------------------------------------------


// FPUBGEquipmentList Begin ---------------------------------------------------------------------------------------

bool FPUBGEquipmentList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FPUBGEquipmentEntry, FPUBGEquipmentList>(Entries, DeltaParams, *this);
}

void FPUBGEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 AddedIndex : AddedIndices)
	{
		FPUBGEquipmentEntry& Entry = Entries[AddedIndex];
		Entry.EquipmentSlotType = (EEquipmentSlotType)AddedIndex;
		Entry.EquipmentManager = EquipmentManager;

		if (Entry.GetItemInstance())
		{
			// ���ø����̼� ����
			Entry.Equipment();
			
			// ��ε� ĳ��Ʈ

		}
	}
}

void FPUBGEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		FPUBGEquipmentEntry& Entry = Entries[ChangedIndex];

		Entry.GetItemInstance() ? Entry.Equipment() : Entry.Unequipment();
		// ��ε� ĳ��Ʈ

	}
}

void FPUBGEquipmentList::Equipment(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance)
{
	FPUBGEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

UPUBGItemInstance* FPUBGEquipmentList::Unequipment(EEquipmentSlotType EquipmentSlotType)
{
	FPUBGEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	UPUBGItemInstance* RemovedItemInstance = Entry.Reset();
	MarkItemDirty(Entry);

	return RemovedItemInstance;
}

// FPUBGEquipmentList End ---------------------------------------------------------------------------------------

// UPUBGEquipmentManagerComponent Begin ---------------------------------------------------------------------------------------
UPUBGEquipmentManagerComponent::UPUBGEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UPUBGEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FPUBGEquipmentEntry>& Entries = EquipmentList.Entries;
		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FPUBGEquipmentEntry& Entry = Entries[i];
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
			Entry.EquipmentManager = this;
			EquipmentList.MarkItemDirty(Entry);
		}
	}
}

void UPUBGEquipmentManagerComponent::UninitializeComponent()
{
	//if (GetOwner() && GetOwner()->HasAuthority())
	//{
	//	TArray<FPUBGEquipmentEntry>& Entries = EquipmentList.Entries;
	//	for (int32 i = 0; i < Entries.Num(); i++)
	//	{
	//		FPUBGEquipmentEntry& Entry = Entries[i];
	//		//Unequip((EEquipmentSlotType)i);
	//	}
	//}

	Super::UninitializeComponent();
}

void UPUBGEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

void UPUBGEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPUBGEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

bool UPUBGEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const FPUBGEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();
		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UPUBGEquipmentManagerComponent::Equipment(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance)
{
	check(GetOwner()->HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || IsValid(ItemInstance) == false)
	{
		return;
	}

	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
	if (EquipableFragment == nullptr)
	{
		return;
	}

	FPUBGEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	Entry.Reset();

	// Ư�� ���� �ܺο��� ������ �ν��Ͻ��� �̸� �޾ƾ���
	if (UPUBGEquipmentManagerComponent::IsFashionSlot(EquipmentSlotType))
	{
		if (EquipmentSlotType == EEquipmentSlotType::Whole)
		{
			FPUBGEquipmentEntry& ClothTopEntry = EquipmentList.Entries[(int32)EEquipmentSlotType::ClothTop];
			if (ClothTopEntry.GetItemInstance() != nullptr)
			{
				Unequipment(EEquipmentSlotType::ClothTop);
			}

			FPUBGEquipmentEntry& ClothBottomEntry = EquipmentList.Entries[(int32)EEquipmentSlotType::ClothBottom];
			if (ClothBottomEntry.GetItemInstance() != nullptr)
			{
				Unequipment(EEquipmentSlotType::ClothBottom);
			}
		}
		else if (EquipmentSlotType == EEquipmentSlotType::ClothTop || EquipmentSlotType == EEquipmentSlotType::ClothBottom)
		{
			FPUBGEquipmentEntry& WholeEntry = EquipmentList.Entries[(int32)EEquipmentSlotType::Whole];
			if (WholeEntry.GetItemInstance() != nullptr)
			{
				Unequipment(EEquipmentSlotType::Whole);
			}
		}
	}

	EquipmentList.Equipment(EquipmentSlotType, ItemInstance);

	// UObject�� FFastArraySerializer���� �ڵ����� ���ø����̼� ��������� �ʾƼ� ���� ���
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		// 1ȸ ȣ�� �ѹ��� ����
		AddReplicatedSubObject(ItemInstance);
	}

	// ���� �ٷ� ���� ����
	if (IsWeaponSlot(EquipmentSlotType))
	{
		if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = GetActiveEquipmentManager())
		{
			APUBGBaseCharacter* Character = GetCharacter();
			const UPUBGItemFragment_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

			EActiveWeaponEquipmentType ActiveEquipmentState = UPUBGActiveEquipmentManagerComponent::ConvertToActiveWeaponEquipmentType(EquipmentSlotType);
			// ���� ������ ���Ⱑ ������ ����
			if (ActiveEquipmentManager->GetCurActiveWeaponEquipmentType() == EActiveWeaponEquipmentType::Unarmed ||
				ActiveEquipmentManager->GetCurActiveWeaponEquipmentType() == EActiveWeaponEquipmentType::Count)
			{
				ActiveEquipmentManager->ChangeEquipState(ActiveEquipmentState);
			}
			// ���Ⱑ ������?
			else
			{
				FName Socket = GetWeaponEquipmentAttachSocket(EquipmentSlotType);
				Entry.SpawnedEquipmentActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket);
			}
		}
	}
}

UPUBGItemInstance* UPUBGEquipmentManagerComponent::Unequipment(EEquipmentSlotType EquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count)
	{
		return nullptr;
	}

	UPUBGItemInstance* RemoveItemInstance = EquipmentList.Unequipment(EquipmentSlotType);

	if (IsUsingRegisteredSubObjectList() && RemoveItemInstance)
	{
		RemoveReplicatedSubObject(RemoveItemInstance);
	}

	return RemoveItemInstance;
}

bool UPUBGEquipmentManagerComponent::IsWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Unarmed <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Throw);
}

bool UPUBGEquipmentManagerComponent::IsArmorSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Helmet <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Vest);
}

bool UPUBGEquipmentManagerComponent::IsUtilitySlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Belt <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Backpack);
}

bool UPUBGEquipmentManagerComponent::IsFashionSlot(EEquipmentSlotType EquipmentSlotType)
{
	return (EEquipmentSlotType::Cap <= EquipmentSlotType) && (EquipmentSlotType <= EEquipmentSlotType::Shoes);
}

bool UPUBGEquipmentManagerComponent::IsActiveEquipWeaponSlot(EEquipmentSlotType EquipmentSlotType)
{
	return EEquipmentSlotType::Unarmed <= EquipmentSlotType && EquipmentSlotType <= EEquipmentSlotType::Throw;
}

APUBGBaseCharacter* UPUBGEquipmentManagerComponent::GetCharacter() const
{
	return Cast<APUBGBaseCharacter>(GetOwner());
}

AController* UPUBGEquipmentManagerComponent::GetController() const
{
	if (APUBGBaseCharacter* Character = GetCharacter())
	{
		return Character->GetController();
	}

	return nullptr;
}

UPUBGActiveEquipmentManagerComponent* UPUBGEquipmentManagerComponent::GetActiveEquipmentManager() const
{
	if (APUBGBaseCharacter* Character = GetCharacter())
	{
		return Character->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>();
	}

	return nullptr;
}

UPUBGAbilitySystemComponent* UPUBGEquipmentManagerComponent::GetPUBGAbilitySystemComponent() const
{
	return Cast<UPUBGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

UPUBGItemInstance* UPUBGEquipmentManagerComponent::GetItemInstance(EEquipmentSlotType EquipmentSlotType)
{
	if (EquipmentSlotType == EEquipmentSlotType::Count)
	{
		return nullptr;
	}

	TArray<FPUBGEquipmentEntry>& Entries = EquipmentList.GetAllEntries();
	const FPUBGEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];

	return Entry.GetItemInstance();
}

TArray<FPUBGEquipmentEntry>& UPUBGEquipmentManagerComponent::GetAllEntries()
{
	return EquipmentList.GetAllEntries();
}

FPUBGEquipmentEntry& UPUBGEquipmentManagerComponent::GetEntry(EEquipmentSlotType EquipmentSlotType)
{
	return EquipmentList.Entries[(int32)EquipmentSlotType];
}

APUBGEquipmentBase* UPUBGEquipmentManagerComponent::GetEntryEquipmentActor(EEquipmentSlotType EquipmentSlotType)
{
	return EquipmentList.Entries[(int32)EquipmentSlotType].GetEquipmentActor();
}

bool UPUBGEquipmentManagerComponent::FindEquipmentSlot(UPUBGItemInstance* InItemInstance, EEquipmentSlotType& EquipmentSlotType)
{
	EquipmentSlotType = EEquipmentSlotType::Count;

	if (InItemInstance == nullptr)
	{
		return false;
	}
	
	const UPUBGItemFragment_Equipable* EquipableFragment = InItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();

	if (EquipableFragment == nullptr)
	{
		return false;
	}

	if (const UPUBGItemFragment_Weapon* WeaponFragment = InItemInstance->FindFragmentByClass<UPUBGItemFragment_Weapon>())
	{
		if (WeaponFragment->WeaponType == EWeaponType::Main)
		{
			// �� ��� �켱 ��ġ
			FPUBGEquipmentEntry& Entry1 = EquipmentList.Entries[(int32)EEquipmentSlotType::Main];
			FPUBGEquipmentEntry& Entry2 = EquipmentList.Entries[(int32)EEquipmentSlotType::Main2];

			if (Entry1.ItemInstance == nullptr)
			{
				EquipmentSlotType = EEquipmentSlotType::Main;
			}
			else if (Entry2.ItemInstance == nullptr)
			{
				EquipmentSlotType = EEquipmentSlotType::Main2;
			}
			else
			{
				EquipmentSlotType = EEquipmentSlotType::Main;
			}
		}
		else
		{
			EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::WeaponTypeConvertToEquipmentSlotType(WeaponFragment->WeaponType);
		}
	}
	else if (const UPUBGItemFragment_Fashion* FashionFragment = InItemInstance->FindFragmentByClass<UPUBGItemFragment_Fashion>())
	{
		EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::FashionTypeConvertToEquipmentSlotType(FashionFragment->FashionType);
	}
	else if (const UPUBGItemFragment_Armor* ArmorFragment = InItemInstance->FindFragmentByClass<UPUBGItemFragment_Armor>())
	{
		EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::ArmorTypeConvertToEquipmentSlotType(ArmorFragment->ArmorType);
	}
	else if (const UPUBGItemFragment_Utility* UtilityFragment = InItemInstance->FindFragmentByClass<UPUBGItemFragment_Utility>())
	{
		EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::UtilityTypeConvertToEquipmentSlotType(UtilityFragment->UtilityType);
	}


	return EquipmentSlotType != EEquipmentSlotType::Count;
}

bool UPUBGEquipmentManagerComponent::IsSocketEmpty(EActiveWeaponEquipmentType ActiveEquipmentState)
{
	EEquipmentSlotType EquipmentSlotType = UPUBGActiveEquipmentManagerComponent::ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(ActiveEquipmentState);
	if (EquipmentSlotType == EEquipmentSlotType::Count)
	{
		return true;
	}

	//if (EquipmentSlotType == EEquipmentSlotType::Unarmed)
	//{
	//	return false;
	//}
	//
	UPUBGItemInstance* ItemInstance = GetItemInstance(EquipmentSlotType);
	return ItemInstance == nullptr;
}

void UPUBGEquipmentManagerComponent::SetWeaponActiveEquipmentSocketToWeaponEquipmentSocket(EEquipmentSlotType EquipmentSlotType)
{
	if (EquipmentSlotType == EEquipmentSlotType::Unarmed || EquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	APUBGBaseCharacter* Character = GetCharacter();
	if (Character == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* MeshComponent = Character->GetMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	FPUBGEquipmentEntry& Entry = EquipmentList.Entries[(int32)EquipmentSlotType];
	APUBGEquipmentBase* SpawnWeaponActor = Entry.SpawnedEquipmentActor;
	UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

	if (ItemInstance == nullptr)
	{
		return;
	}

	const UPUBGItemFragment_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

	if (SpawnWeaponActor && AttachmentFragment)
	{
		FName SocketName = GetWeaponEquipmentAttachSocket(EquipmentSlotType);
		SpawnWeaponActor->SetActorTransform(MeshComponent->GetSocketTransform(SocketName));
		FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
		SpawnWeaponActor->AttachToComponent(MeshComponent, AttachmentTransformRules, SocketName);
	}
}

FName UPUBGEquipmentManagerComponent::GetWeaponEquipmentAttachSocket(EEquipmentSlotType EquipmentSlotType) const
{
	FName SocketName = NAME_None;

	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponent();

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Main:
	{
		bool Result = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Equip_Backpack);
		SocketName = (Result) ? FName(TEXT("Socket_BackLeft_Backpack")) : FName(TEXT("Socket_BackLeft_Normal"));
		break;
	}
	case EEquipmentSlotType::Main2:
	{
		bool Result = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Equip_Backpack);
		SocketName = (Result) ? FName(TEXT("Socket_BackRight_Backpack")) : FName(TEXT("Socket_BackRight_Normal"));
		break;
	}
	case EEquipmentSlotType::Sub:
		SocketName = FName(TEXT("Socket_ThighRight_Sub"));
		break;
	case EEquipmentSlotType::Etc:
		SocketName = FName(TEXT("Socket_WaistBack_Etc"));
		break;
	case EEquipmentSlotType::Throw:
		SocketName = FName(TEXT("Socket_WaistRight_Throw"));
		break;
	}

	return SocketName;
}

void UPUBGEquipmentManagerComponent::ReplaceWeaponEquipmentSocket()
{
	APUBGBaseCharacter* Character = GetCharacter();

	if (Character == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* MeshComponent = Character->GetMesh();
	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponent();
	bool IsEquipBackpack = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Equip_Backpack);

	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = GetActiveEquipmentManager();
	EEquipmentSlotType CurEquipmentSlotType = UPUBGActiveEquipmentManagerComponent::ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(ActiveEquipmentManager->GetCurActiveWeaponEquipmentType());
	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);

	FPUBGEquipmentEntry& MainEntry = EquipmentList.Entries[(int32)EEquipmentSlotType::Main];
	APUBGEquipmentBase* WeaponMainActor = MainEntry.SpawnedEquipmentActor;
	if (CurEquipmentSlotType != EEquipmentSlotType::Main && WeaponMainActor)
	{
		FName SocketName = (IsEquipBackpack) ? FName(TEXT("Socket_BackLeft_Backpack")) : FName(TEXT("Socket_BackLeft_Normal"));
		FTransform NewTransform = MeshComponent->GetSocketTransform(SocketName);
		WeaponMainActor->SetActorTransform(NewTransform);
		WeaponMainActor->AttachToComponent(MeshComponent, AttachmentTransformRules, SocketName);
	}


	FPUBGEquipmentEntry& Main2Entry = EquipmentList.Entries[(int32)EEquipmentSlotType::Main2];
	APUBGEquipmentBase* WeaponMain2Actor = Main2Entry.SpawnedEquipmentActor;
	if (CurEquipmentSlotType != EEquipmentSlotType::Main2 && WeaponMain2Actor)
	{
		FName SocketName = (IsEquipBackpack) ? FName(TEXT("Socket_BackRight_Backpack")) : FName(TEXT("Socket_BackRight_Normal"));
		FTransform NewTransform = MeshComponent->GetSocketTransform(SocketName);
		WeaponMain2Actor->SetActorTransform(NewTransform);
		WeaponMain2Actor->AttachToComponent(MeshComponent, AttachmentTransformRules, SocketName);
	}
}


// UPUBGEquipmentManagerComponent End ---------------------------------------------------------------------------------------