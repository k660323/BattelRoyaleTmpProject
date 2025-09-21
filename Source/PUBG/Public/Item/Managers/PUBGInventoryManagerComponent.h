// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PUBGInventoryManagerComponent.generated.h"

class UPUBGItemInstance;
struct FPUBGInventoryList;
class UPUBGInventoryManagerComponent;
class UPUBGItemTemplate;

USTRUCT(BlueprintType)
struct FPUBGInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FPUBGInventoryEntry()
		: InventoryManager(nullptr), ItemIndex(0), ItemInstance(nullptr), ItemCount(0) {}

public:
	void Init(UPUBGItemInstance* InItemInstance, int32 InItemCount);
	UPUBGItemInstance* Clear();

public:
	UPUBGItemInstance* GetItemInstance() { return ItemInstance; }
	int32 GetItemCount() { return ItemCount; }
	int32 GetItemIndex() { return ItemIndex; }

public:
	UPROPERTY(NotReplicated)
	UPUBGInventoryManagerComponent* InventoryManager;

private:
	friend struct FPUBGInventoryList;
	friend class UPUBGInventoryManagerComponent;

	UPROPERTY(NotReplicated)
	int32 ItemIndex;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPUBGItemInstance> ItemInstance;

	UPROPERTY(VisibleAnywhere)
	int32 ItemCount;
};

USTRUCT(BlueprintType)
struct FPUBGInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FPUBGInventoryList() : InventoryManager(nullptr) {}
	FPUBGInventoryList(UPUBGInventoryManagerComponent* InOwnerComponent) : InventoryManager(InOwnerComponent){}

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

public:
	const TArray<FPUBGInventoryEntry>& GetAllInventoryEntry() const;

private:
	friend class UPUBGInventoryManagerComponent;

	UPROPERTY(VisibleAnywhere)
	TArray<FPUBGInventoryEntry> Entries;


	UPROPERTY(NotReplicated)
	UPUBGInventoryManagerComponent* InventoryManager;
};

template<>
struct TStructOpsTypeTraits<FPUBGInventoryList> : public TStructOpsTypeTraitsBase2<FPUBGInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UPUBGInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPUBGItemInstance* GetItemInstance(int32 ItemIndex);
	TArray<FPUBGInventoryEntry>& GetAllEntries();
	FPUBGInventoryEntry& GetEntry(int32 ItemIndex);

	// �뷮 üũ
	int32 CanAddItemByWeight(const UPUBGItemTemplate& ItemTemplate, int32 Count) const;
	int32 CanAddItemByWeight(UPUBGItemInstance* ItemInstance, int32 Count) const;

	// ������ �߰��Ҽ� �ִ��� �ùķ��̼Ǹ� (������ �� ������ �־�� true) (false�� �������� ���� ���ϰų� �ļ�ó�� �ʿ�)
	bool CanAddItem(UPUBGItemInstance* InItemInstance, int32& InCount);
	// ������ �߰� (������ �� ������ �־�� true) (false�� �����ۤ��� ���� ���ϰų� �ļ�ó�� �ʿ�)
	bool TryAddItem(UPUBGItemInstance* InItemInstance, int32& InCount);

	// Ư�� ������ ���ø� Ÿ�� ã�Ƽ� �ε��� ��ȯ
	int32 SearchItemTemplateType(int32& StartIndex, const UPUBGItemTemplate& TargetItemTemplate) const;

	// �� ��Ʈ�� ���� ã�Ƽ� �ε��� ��ȯ
	int32 SearchEmptyItemInstanceEntry(int32& StartIndex) const;

	int32 CanRemoveItem(int32 ItemIndex, int32 ItemCount);
	// ������ ����
	UPUBGItemInstance* TryRemoveItem(int32 ItemIndex, int32 ItemCount);

	bool TryRemoveItem(UPUBGItemInstance* ItemInstance);

	int32 GetItemCount(int32 ItemIndex);




	UFUNCTION(BlueprintCallable)
	void PrintAllItemList();

	float GetCurrentCapacityWeight() const;
	float GetMaxCapacityWeight() const;
	float GetAdditionalMaxCapacityWeight() const;
	float GetTotalAcceptableWeight() const;

public:
	UFUNCTION()
	void OnRep_CurrentCapacityWeight();

	UFUNCTION()
	void OnRep_MaxAcceptableWeight();


	UFUNCTION()
	void OnAdditionalMaxCapacityWeight(FGameplayTag Channel, const FPUBGAdditionalValueMessage& Message);

private:
	friend struct FPUBGInventoryList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentCapacityWeight, meta = (AllowPrivateAccess = "true"))
	float CurrentCapacityWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAcceptableWeight, meta = (AllowPrivateAccess = "true"))
	float MaxCapacityWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AdditionalMaxCapacityWeight;

	UPROPERTY()
	FGameplayMessageListenerHandle MessageListenerHandle;

	// �־��� ��� ���� (���� �뷮 + ��Ʈ + 3���� + ����)
	UPROPERTY(VisibleAnywhere)
	int32 InventoryCapacity;

	UPROPERTY(VisibleAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FPUBGInventoryList InventoryList;
};
