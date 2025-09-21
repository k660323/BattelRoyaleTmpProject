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

	// 용량 체크
	int32 CanAddItemByWeight(const UPUBGItemTemplate& ItemTemplate, int32 Count) const;
	int32 CanAddItemByWeight(UPUBGItemInstance* ItemInstance, int32 Count) const;

	// 아이템 추가할수 있는지 시뮬레이션만 (아이템 다 넣을수 있어야 true) (false면 아이템을 넣지 못하거나 후속처리 필요)
	bool CanAddItem(UPUBGItemInstance* InItemInstance, int32& InCount);
	// 아이템 추가 (아이템 다 넣을수 있어야 true) (false면 아이템ㅇ르 넣지 못하거나 후속처리 필요)
	bool TryAddItem(UPUBGItemInstance* InItemInstance, int32& InCount);

	// 특정 아이템 템플릿 타입 찾아서 인덱스 반환
	int32 SearchItemTemplateType(int32& StartIndex, const UPUBGItemTemplate& TargetItemTemplate) const;

	// 빈 엔트리 소켓 찾아서 인덱스 반환
	int32 SearchEmptyItemInstanceEntry(int32& StartIndex) const;

	int32 CanRemoveItem(int32 ItemIndex, int32 ItemCount);
	// 아이템 삭제
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

	// 최악의 경우 산정 (기존 용량 + 벨트 + 3가방 + 조끼)
	UPROPERTY(VisibleAnywhere)
	int32 InventoryCapacity;

	UPROPERTY(VisibleAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FPUBGInventoryList InventoryList;
};
