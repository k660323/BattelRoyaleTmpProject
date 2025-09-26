// Kakanu Portfolio Project


#include "Item/Managers/PUBGWeaponAccManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Actors/PUBGEquipmentBase.h"
#include "Item/PUBGItemInstance.h"
#include "Item/PUBGItemTemplate.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Engine/ActorChannel.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGItemData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGWeaponAccManagerComponent)

void FPUBGWeaponAccEntry::Init(UPUBGItemInstance* InItemInstance)
{
	check(InItemInstance);

	if (ItemInstance == InItemInstance)
	{
		return;
	}

	// ���� ItemInstance �� ��� ó���� �Ǵ°�?
	if (ItemInstance)
	{
		// ���־� ���� (����, ���� ������)
		UnEquip();
	}

	ItemInstance = InItemInstance;

	// ������ ����
	if (InItemInstance)
	{
		Equip();
	}
}

UPUBGItemInstance* FPUBGWeaponAccEntry::Reset()
{
	// ���� ����
	if (ItemInstance)
	{
		UnEquip();
	}

	UPUBGItemInstance* RemoveItemInstance = ItemInstance;
	ItemInstance = nullptr;

	return RemoveItemInstance;
}

void FPUBGWeaponAccEntry::Equip()
{
	APUBGBaseCharacter* Character = WeaponAccManager->GetOwnerAsCharacter();

	if (Character == nullptr)
	{
		return;
	}


	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
	if (EquipableFragment == nullptr)
	{
		return;
	}

	if (WeaponAccManager->GetOwner()->HasAuthority())
	{
		if (UPUBGAbilitySystemComponent* ASC = WeaponAccManager->GetPUBGAbilitySystemComponent())
		{
			// ��� �ɷ�ġ ����Ʈ ����
			ASC->RemoveActiveGameplayEffect(BaseStatHandle);

			// �±� �ο�
			if (EquipableFragment->GrantedTags.IsEmpty() == false)
			{
				GrantTagsHandle = ASC->AddDynamicTagsGameplayEffect(EquipableFragment->GrantedTags);
			}

			// ��� �ɷ�ġ ����
			if (ItemInstance->GetStatContainer().GetStacks().Num() > 0)
			{
				const  TSubclassOf<UGameplayEffect> AttributeModifierGE = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("Attribute_Modifier"));
				check(AttributeModifierGE);

				const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(*AttributeModifierGE, 1.f, ContextHandle);

				// SetByCaller �� 0���� �ʱ�ȭ
				const TSharedPtr<FGameplayEffectSpec>& SpecData = SpecHandle.Data;

				for (const FGameplayModifierInfo& ModifierInfo : SpecData->Def->Modifiers)
				{
					SpecData->SetSetByCallerMagnitude(ModifierInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag, 0.0f);
				}

				for (const FPUBGGameplayTagStack& Stack : ItemInstance->GetStatContainer().GetStacks())
				{
					SpecData->SetSetByCallerMagnitude(Stack.GetStackTag(), Stack.GetStackCount());
				}

				BaseStatHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			}
		}
	}
	else
	{
		if (EquipableFragment->EquipmentType == EEquipmentType::GunAccessory)
		{
			if (const UPUBGItemFragment_GunAcc* GunFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>())
			{
				APUBGEquipmentBase* EquipmentActor = WeaponAccManager->GetOwnerAsEquipment();
				const UPUBGItemTemplate& WeaponItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(EquipmentActor->GetItemTempalteID());
				TSubclassOf<UPUBGItemTemplate> WeaponItemTemplateClass = WeaponItemTemplate.GetClass();

				TSoftObjectPtr<UStaticMesh> AttachWeaponAccMesh = GunFragment->GunAccMeshMap[WeaponItemTemplateClass];
				if (AttachWeaponAccMesh)
				{
					UStaticMesh* LoadedStaticMesh = UPUBGAssetManager::GetAssetByPath(AttachWeaponAccMesh);

					if (LoadedStaticMesh)
					{
						WeaponAccMesh = NewObject<UStaticMeshComponent>(EquipmentActor, *(FString::Printf(TEXT("SKM_")) + StaticEnum<EGunAccType>()->GetValueAsString(WeaponAccType)));
						WeaponAccMesh->SetupAttachment(EquipmentActor->GetTPSEquipmentMesh()); // �θ� ����, ���� ��ǥ (0,0,0) snaptotarget
						WeaponAccMesh->SetCollisionProfileName(TEXT("NoCollision"));
						WeaponAccMesh->SetStaticMesh(LoadedStaticMesh);
						WeaponAccMesh->bWantsInitializeComponent = true;
						WeaponAccMesh->RegisterComponent();

						FAttachmentTransformRules TransformRules = FAttachmentTransformRules(
							EAttachmentRule::SnapToTarget,
							EAttachmentRule::KeepWorld,
							EAttachmentRule::KeepRelative,
							true
						);

						WeaponAccMesh->AttachToComponent(EquipmentActor->GetTPSEquipmentMesh(), TransformRules, GunFragment->AttachSocket);
					}
				}
			}
		}
	}
}

void FPUBGWeaponAccEntry::UnEquip()
{
	APUBGBaseCharacter* Character = WeaponAccManager->GetOwnerAsCharacter();

	if (Character == nullptr)
	{
		return;
	}

	if (WeaponAccManager->GetOwner()->HasAuthority())
	{
		// �����Ƽ, ����Ʈ, ��Ʈ����Ʈ ����
		if (UPUBGAbilitySystemComponent* ASC = WeaponAccManager->GetPUBGAbilitySystemComponent())
		{
			// ��� �ɷ�ġ ����Ʈ ����
			ASC->RemoveActiveGameplayEffect(BaseStatHandle);
			BaseStatHandle.Invalidate();
			// �±� ����
			ASC->RemoveActiveGameplayEffect(GrantTagsHandle);
			GrantTagsHandle.Invalidate();
		}
	}
	else
	{
		if (WeaponAccMesh)
		{
			WeaponAccMesh->DestroyComponent();
			WeaponAccMesh = nullptr;
		}
	}
}

bool FPUBGWeaponAccList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FPUBGWeaponAccEntry, FPUBGWeaponAccList>(Entries, DeltaParams, *this);
}

void FPUBGWeaponAccList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 AddedIndex : AddedIndices)
	{
		FPUBGWeaponAccEntry& Entry = Entries[AddedIndex];
		Entry.WeaponAccManager = WeaponAccManager;
		Entry.WeaponAccType = (EGunAccType)AddedIndex;

		if (Entry.GetItemInstance())
		{
			Entry.Equip();
		}
	}
}

void FPUBGWeaponAccList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		FPUBGWeaponAccEntry& Entry = Entries[ChangedIndex];

		Entry.GetItemInstance() ? Entry.Equip() : Entry.UnEquip();
	}
}

void FPUBGWeaponAccList::EquipAcc(EGunAccType AccType, UPUBGItemInstance* InItemInstance)
{
	FPUBGWeaponAccEntry& Entry = Entries[(int32)AccType];
	Entry.Init(InItemInstance);
	MarkItemDirty(Entry);
}

UPUBGItemInstance* FPUBGWeaponAccList::UnEquipAcc(EGunAccType AccType)
{
	FPUBGWeaponAccEntry& Entry = Entries[(int32)AccType];
	UPUBGItemInstance* RemovedItemInstance = Entry.Reset();
	MarkItemDirty(Entry);

	return RemovedItemInstance;
}

// Sets default values for this component's properties
UPUBGWeaponAccManagerComponent::UPUBGWeaponAccManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	WeaponAccList(this)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UPUBGWeaponAccManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// ���� ����
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FPUBGWeaponAccEntry>& Entries = WeaponAccList.Entries;
		Entries.SetNum((int32)EGunAccType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FPUBGWeaponAccEntry& Entry = Entries[i];
			Entry.WeaponAccType = (EGunAccType)i;
			Entry.WeaponAccManager = this;

			WeaponAccList.MarkItemDirty(Entry);
		}
	}
}

void UPUBGWeaponAccManagerComponent::UninitializeComponent()
{
	// ���� ����
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FPUBGWeaponAccEntry>& Entries = WeaponAccList.Entries;

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			UnEquipAcc((EGunAccType)i);
		}
	}

	Super::UninitializeComponent();
}

void UPUBGWeaponAccManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, WeaponAccList, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UPUBGWeaponAccManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPUBGWeaponAccEntry& Entry : WeaponAccList.Entries)
		{
			UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}
}

bool UPUBGWeaponAccManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FPUBGWeaponAccEntry& Entry : WeaponAccList.Entries)
	{
		UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}


void UPUBGWeaponAccManagerComponent::EquipAcc(EGunAccType AccType, UPUBGItemInstance* InItemInstance)
{
	check(GetOwner()->HasAuthority());

	if (AccType == EGunAccType::Count || InItemInstance == nullptr)
	{
		return;
	}

	WeaponAccList.EquipAcc(AccType, InItemInstance);

	// UObject�� FFastArraySerializer���� �ڵ����� ���ø����̼� ��������� �ʾƼ� ���� ���
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && InItemInstance)
	{
		// 1ȸ ȣ�� �ѹ��� ����
		AddReplicatedSubObject(InItemInstance);
	}
}

UPUBGItemInstance* UPUBGWeaponAccManagerComponent::UnEquipAcc(EGunAccType AccType)
{
	check(GetOwner()->HasAuthority());

	if (AccType == EGunAccType::Count)
	{
		return nullptr;
	}

	UPUBGItemInstance* RemovedItemInstance = WeaponAccList.UnEquipAcc(AccType);

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && RemovedItemInstance)
	{
		RemoveReplicatedSubObject(RemovedItemInstance);
	}

	return RemovedItemInstance;
}

UPUBGAbilitySystemComponent* UPUBGWeaponAccManagerComponent::GetPUBGAbilitySystemComponent() const
{
	return Cast<UPUBGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwnerAsCharacter()));
}

UPUBGItemInstance* UPUBGWeaponAccManagerComponent::GetWeaponAccItemInstance(EGunAccType WeaponAccType)
{
	if (WeaponAccType == EGunAccType::Count)
	{
		return nullptr;
	}

	return WeaponAccList.Entries[(int32)WeaponAccType].GetItemInstance();
}

TArray<FPUBGWeaponAccEntry>& UPUBGWeaponAccManagerComponent::GetAllEntry()
{
	return WeaponAccList.Entries;
}

FPUBGWeaponAccEntry& UPUBGWeaponAccManagerComponent::GetEntry(EGunAccType WeaponAccType)
{
	return WeaponAccList.Entries[(int32)WeaponAccType];
}

APUBGEquipmentBase* UPUBGWeaponAccManagerComponent::GetOwnerAsEquipment() const
{
	// ���� ����
	if (GetOwner() == nullptr)
	{
		return nullptr;
	}

	return Cast<APUBGEquipmentBase>(GetOwner());
}

APUBGBaseCharacter* UPUBGWeaponAccManagerComponent::GetOwnerAsCharacter() const
{
	// ���� ���� -> �÷��̾� ����
	if (GetOwner() == nullptr || GetOwner()->GetOwner() == nullptr)
	{
		return nullptr;
	}

	return Cast<APUBGBaseCharacter>(GetOwner()->GetOwner());
}

