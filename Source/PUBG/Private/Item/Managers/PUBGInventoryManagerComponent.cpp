// Kakanu Portfolio Project


#include "Item/Managers/PUBGInventoryManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Data/PUBGItemData.h"
#include "Item/PUBGItemInstance.h"
#include "Item/PUBGItemTemplate.h"
#include "Messages/PUBGAdditionalValueMessage.h"
#include "PUBGGameplayTags.h"
#include "GameplayTagContainer.h"

void FPUBGInventoryEntry::Init(UPUBGItemInstance* InItemInstance, int32 InItemCount)
{
	check(InItemInstance && InItemCount > 0);

	ItemInstance = InItemInstance;

	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(InItemInstance->GetItemTemplateID());
	ItemCount = FMath::Clamp(InItemCount, 1, ItemTemplate.MaxStackCount);
}

UPUBGItemInstance* FPUBGInventoryEntry::Clear()
{
	UPUBGItemInstance* RemovedItemInstance = ItemInstance;
	ItemInstance = nullptr;
	ItemCount = 0;

	return RemovedItemInstance;
}

//-----------------------------------------------------------------------

bool FPUBGInventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FPUBGInventoryEntry, FPUBGInventoryList>(Entries, DeltaParams, *this);
}

void FPUBGInventoryList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	// �ʱ�ȭ
	for (int32 AddedIndex : AddedIndices)
	{
		FPUBGInventoryEntry& Entry = Entries[AddedIndex];
		Entry.InventoryManager = InventoryManager;
		Entry.ItemIndex = AddedIndex;

		if (Entry.ItemInstance)
		{
			// ��ε� ĳ��Ʈ
			
		}
	}
}

void FPUBGInventoryList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		FPUBGInventoryEntry& Entry = Entries[ChangedIndex];
		if (Entry.ItemInstance)
		{
			// ��ε� ĳ��Ʈ
		}
		else
		{
			// ��ε� ĳ��Ʈ

		}
	}
}

const TArray<FPUBGInventoryEntry>& FPUBGInventoryList::GetAllInventoryEntry() const
{
	return Entries;
}

//-----------------------------------------------------------------------

// Sets default values for this component's properties
UPUBGInventoryManagerComponent::UPUBGInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentCapacityWeight(0.0f)
	, MaxCapacityWeight(50.0f)
	, AdditionalMaxCapacityWeight(0.0f)
	, InventoryCapacity(64)
	, InventoryList(this)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UPUBGInventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (!GetWorld() || (GetWorld() && (GetWorld()->WorldType == EWorldType::EditorPreview)))
	{
		return;
	}

	// ������ ���� 
	// �迭 ����
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FPUBGInventoryEntry>& Entries = InventoryList.Entries;
		Entries.SetNum(InventoryCapacity);

		for (int32 index = 0; index < Entries.Num(); index++)
		{
			FPUBGInventoryEntry& Entry = Entries[index];
			Entry.ItemIndex = index;
			Entry.InventoryManager = this;
			InventoryList.MarkItemDirty(Entry);
		}
	}

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageListenerHandle = MessageSystem.RegisterListener(PUBGGameplayTags::Message_AdditionalValue, this, &ThisClass::OnAdditionalMaxCapacityWeight);
}

void UPUBGInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, InventoryList, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentCapacityWeight, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxCapacityWeight, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UPUBGInventoryManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(MessageListenerHandle);

	Super::EndPlay(EndPlayReason);
}

UPUBGItemInstance* UPUBGInventoryManagerComponent::GetItemInstance(int32 ItemIndex)
{
	if (ItemIndex < 0 || ItemIndex >= InventoryList.Entries.Num())
	{
		return nullptr;
	}

	return InventoryList.Entries[ItemIndex].GetItemInstance();
}

TArray<FPUBGInventoryEntry>& UPUBGInventoryManagerComponent::GetAllEntries()
{
	return InventoryList.Entries;
}

FPUBGInventoryEntry& UPUBGInventoryManagerComponent::GetEntry(int32 ItemIndex)
{
	return InventoryList.Entries[ItemIndex];
}

int32 UPUBGInventoryManagerComponent::CanAddItemByWeight(const UPUBGItemTemplate& ItemTemplate, int32 Count) const
{
	int32 PushableItemCount = (GetTotalAcceptableWeight() - CurrentCapacityWeight) / ItemTemplate.Weight;

	return FMath::Min(Count, PushableItemCount);
}

int32 UPUBGInventoryManagerComponent::CanAddItemByWeight(UPUBGItemInstance* ItemInstance, int32 Count) const
{
	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());

	int32 PushableItemCount = GetTotalAcceptableWeight() / ItemTemplate.Weight;

	return FMath::Min(Count, PushableItemCount);
}

bool UPUBGInventoryManagerComponent::CanAddItem(UPUBGItemInstance* InItemInstance, int32& InCount)
{
	if (IsValid(InItemInstance) == false)
	{
		return false;
	}

	int32 ItemTemplateID = InItemInstance->GetItemTemplateID();
	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);

	// ���Է� ������ �ִ� ������ ���� ��ȯ
	int32 Count = CanAddItemByWeight(ItemTemplate, InCount);

	if (Count == 0)
	{
		return false;
	}

	// ������ ��Ī
	int32 SearchIndex = 0;
	int32 ItemPushCount = 0;

	// ������ ������ �ִ� ������
	if (ItemTemplate.MaxStackCount > 1)
	{
		// ���� �����۵��� ã�� �������� ä�� �ֱ�
		while (SearchIndex < InventoryCapacity && Count != 0)
		{
			int32 FindIndex = SearchItemTemplateType(SearchIndex, ItemTemplate);

			FPUBGInventoryEntry& Entry = InventoryList.Entries[FindIndex];
			int32 PreItemCount = Entry.ItemCount;
			// ������ �߰��Ҽ� ������ �߰�
			int32 TempItemCount = FMath::Min(Count + Entry.ItemCount, ItemTemplate.MaxStackCount);

			// �ջ�� ������ ����
			int32 AddedItemCount = TempItemCount - PreItemCount;
			ItemPushCount += AddedItemCount;

			// �߰��ϰ� ������ �����ۼ� ��ġ
			Count = FMath::Max(0, Count - AddedItemCount);

			SearchIndex++;
		}
	}

	// ������� ������ ã��
	while (SearchIndex < InventoryCapacity && Count != 0)
	{
		int32 FindIndex = SearchEmptyItemInstanceEntry(SearchIndex);
		// ã��
		if (FindIndex != -1)
		{
			// ������ ����
			int32 TempItemCount = FMath::Min(Count, ItemTemplate.MaxStackCount);

			// �ջ�� ������ ����
			int32 AddedItemCount = TempItemCount;
			ItemPushCount += AddedItemCount;

			// �߰��ϰ� ������ �����ۼ� ��ġ
			Count = FMath::Max(0, Count - AddedItemCount);

			SearchIndex++;
		}
	}

	// ������ ��ȯ
	InCount = InCount - ItemPushCount;

	return (InCount == 0);
}

bool UPUBGInventoryManagerComponent::TryAddItem(UPUBGItemInstance* InItemInstance, int32& InCount)
{
	check(GetOwner()->HasAuthority());

	if (IsValid(InItemInstance) == false)
	{
		return false;
	}

	int32 ItemTemplateID = InItemInstance->GetItemTemplateID();
	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);
	
	// ���Է� ������ �ִ� ������ ���� ��ȯ
	int32 Count = CanAddItemByWeight(ItemTemplate, InCount);
	
	if (Count == 0)
	{
		return false;
	}

	// ������ ��Ī
	int32 SearchIndex = 0;
	int32 ItemPushCount = 0;

	// ������ ������ �ִ� ������
	if (ItemTemplate.MaxStackCount > 1)
	{
		// ���� �����۵��� ã�� �������� ä�� �ֱ�
		while (SearchIndex < InventoryCapacity && Count != 0)
		{
			int32 FindIndex = SearchItemTemplateType(SearchIndex, ItemTemplate);

			// ã��
			if (FindIndex != -1)
			{
				FPUBGInventoryEntry& Entry = InventoryList.Entries[FindIndex];
				int32 PreItemCount = Entry.ItemCount;
				// ������ �߰��Ҽ� ������ �߰�
				Entry.ItemCount = FMath::Min(Count + Entry.ItemCount,ItemTemplate.MaxStackCount);
				InventoryList.MarkItemDirty(Entry);
				
				// �ջ�� ������ ����
				int32 AddedItemCount = Entry.ItemCount - PreItemCount;
				ItemPushCount += AddedItemCount;

				// �߰��ϰ� ������ �����ۼ� ��ġ
				Count = FMath::Max(0, Count - AddedItemCount);

				SearchIndex++;
			}
		}
	}

	// ������� ������ ã�� �ֱ�
	SearchIndex = 0;
	while (SearchIndex < InventoryCapacity && Count != 0)
	{
		int32 FindIndex = SearchEmptyItemInstanceEntry(SearchIndex);
		// ã��
		if (FindIndex != -1)
		{
			// ������ ����
			FPUBGInventoryEntry& Entry = InventoryList.Entries[FindIndex];
			UPUBGItemInstance* NewItemInstance = NewObject<UPUBGItemInstance>();
			NewItemInstance->Init(ItemTemplateID);
			Entry.ItemInstance = NewItemInstance;
			Entry.ItemCount = FMath::Min(Count, ItemTemplate.MaxStackCount);
			InventoryList.MarkItemDirty(Entry);
			
			// �ջ�� ������ ����
			int32 AddedItemCount = Entry.ItemCount;
			ItemPushCount += AddedItemCount;

			// �߰��ϰ� ������ �����ۼ� ��ġ
			Count = FMath::Max(0, Count - AddedItemCount);

			SearchIndex++;
		}
	}

	// ���� ���� �ջ�
	CurrentCapacityWeight += ItemPushCount * ItemTemplate.Weight;
	// ������ ��ȯ
	InCount = InCount - ItemPushCount;

	return (InCount == 0);
}

int32 UPUBGInventoryManagerComponent::SearchItemTemplateType(int32& StartIndex, const UPUBGItemTemplate& TargetItemTemplate) const
{
	for (; StartIndex < InventoryCapacity; StartIndex++)
	{
		const FPUBGInventoryEntry& Entry = InventoryList.Entries[StartIndex];
		if (Entry.ItemInstance)
		{
			const UPUBGItemTemplate& OtherItemTempalte = UPUBGItemData::Get().FindItemTemplateByID(Entry.ItemInstance->GetItemTemplateID());
			if (OtherItemTempalte.GetClass() == TargetItemTemplate.GetClass())
			{
				return StartIndex;
			}
		}
	}

	return -1;
}

int32 UPUBGInventoryManagerComponent::SearchEmptyItemInstanceEntry(int32& StartIndex) const
{
	for (; StartIndex < InventoryCapacity; StartIndex++)
	{
		const FPUBGInventoryEntry& Entry = InventoryList.Entries[StartIndex];
		if (Entry.ItemInstance == nullptr)
		{
			return StartIndex;
		}
	}

	return -1;
}

int32 UPUBGInventoryManagerComponent::CanRemoveItem(int32 ItemIndex, int32 ItemCount)
{
	int32 MoveableItemCount = 0;
	
	FPUBGInventoryEntry& Entry = InventoryList.Entries[ItemIndex];
	MoveableItemCount = (ItemCount <= Entry.ItemCount) ? ItemCount : Entry.ItemCount;

	return MoveableItemCount;
}

UPUBGItemInstance* UPUBGInventoryManagerComponent::TryRemoveItem(int32 ItemIndex, int32 ItemCount)
{
	check(GetOwner()->HasAuthority());

	if (ItemCount == 0)
	{
		return nullptr;
	}

	FPUBGInventoryEntry& Entry = InventoryList.Entries[ItemIndex];
	if (IsValid(Entry.GetItemInstance()) == false)
	{
		return nullptr;
	}
	Entry.ItemCount = FMath::Max(0, Entry.ItemCount - ItemCount);
	int32 ItemTemplateID = Entry.ItemInstance->GetItemTemplateID();
	

	UPUBGItemInstance* NewItemInstance = nullptr;

	// ������ ī���Ͱ� 0���ϸ� ���� ������� �ν��Ͻ� ��ȯ
	if (Entry.ItemCount <= 0)
	{
		NewItemInstance = Entry.Clear();

		if (IsUsingRegisteredSubObjectList() && NewItemInstance)
		{
			RemoveReplicatedSubObject(NewItemInstance);
		}
	}
	// ������ ī���Ͱ� 1�̻��̸� ���ο� ������ �ν��Ͻ��� �����ؼ� ��ȯ
	else
	{
		NewItemInstance = NewObject<UPUBGItemInstance>();
		NewItemInstance->Init(ItemTemplateID);
	}

	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);
	int32 RemovedItemCount = (ItemCount <= Entry.ItemCount) ? ItemCount : Entry.ItemCount;
	CurrentCapacityWeight -= RemovedItemCount * ItemTemplate.Weight;
	
	InventoryList.MarkItemDirty(Entry);

	return NewItemInstance;
}

bool UPUBGInventoryManagerComponent::TryRemoveItem(UPUBGItemInstance* ItemInstance)
{
	return false;
}

int32 UPUBGInventoryManagerComponent::GetItemCount(int32 ItemIndex)
{
	return InventoryList.Entries[ItemIndex].GetItemCount();
}

void UPUBGInventoryManagerComponent::PrintAllItemList()
{
	for (int32 i = 0; i < InventoryList.Entries.Num(); i++)
	{
		FPUBGInventoryEntry& Entry = InventoryList.Entries[i];
		if (Entry.ItemInstance != nullptr)
		{

			int32 ItemIndex = Entry.ItemIndex;
			int32 ItemTemplateID = Entry.GetItemInstance()->GetItemTemplateID();
			int32 ItemCount = Entry.GetItemCount();

			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green,
				FString::Printf(TEXT("Index : %d, ID : %d, ItemCount : %d"), ItemIndex, ItemTemplateID, ItemCount)
			);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue,
		FString::Printf(TEXT("CurCapacityWeight : %f, TotalAcceptableWeight : %f, AdditionalMaxCapacityWeight : %f"),
			CurrentCapacityWeight, GetTotalAcceptableWeight(), AdditionalMaxCapacityWeight)
	);
}

float UPUBGInventoryManagerComponent::GetCurrentCapacityWeight() const
{
	return CurrentCapacityWeight;
}

float UPUBGInventoryManagerComponent::GetMaxCapacityWeight() const
{
	return MaxCapacityWeight;
}

float UPUBGInventoryManagerComponent::GetAdditionalMaxCapacityWeight() const
{
	return AdditionalMaxCapacityWeight;
}

float UPUBGInventoryManagerComponent::GetTotalAcceptableWeight() const
{
	return MaxCapacityWeight + AdditionalMaxCapacityWeight;
}

void UPUBGInventoryManagerComponent::OnRep_CurrentCapacityWeight()
{
	// UI

}

void UPUBGInventoryManagerComponent::OnRep_MaxAcceptableWeight()
{
	// UI

}

void UPUBGInventoryManagerComponent::OnAdditionalMaxCapacityWeight(FGameplayTag Channel, const FPUBGAdditionalValueMessage& Message)
{
	AdditionalMaxCapacityWeight = Message.ChangedAdditionalValue;
	// UI

}