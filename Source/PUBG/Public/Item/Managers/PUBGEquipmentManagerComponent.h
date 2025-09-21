// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Components/ActorComponent.h"
#include "PUBGEquipmentManagerComponent.generated.h"

class UPUBGEquipmentManagerComponent;
class UPUBGInventoryManagerComponent;
class UPUBGItemManagerComponent;
class UPUBGItemInstance;
class APUBGBaseCharacter;
class AController;
class UPUBGActiveEquipmentManagerComponent;
class APUBGEquipmentBase;
class UPUBGAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FPUBGEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	FPUBGEquipmentEntry() : 
		ItemInstance(nullptr),
		EquipmentSlotType(EEquipmentSlotType::Count),
		EquipmentManager(nullptr),
		SpawnedEquipmentActor(nullptr)
	{}

private:
	void Init(UPUBGItemInstance* InItemInstance);
	UPUBGItemInstance* Reset();

	void Equipment();
	void Unequipment();

public:
	FORCEINLINE UPUBGItemInstance* GetItemInstance() const { return ItemInstance; }

	FORCEINLINE void SetEquipmentActor(APUBGEquipmentBase* InEquipmentActor) { SpawnedEquipmentActor = InEquipmentActor; }
	FORCEINLINE APUBGEquipmentBase* GetEquipmentActor() { return SpawnedEquipmentActor; }

private:
	UPROPERTY()
	TObjectPtr<UPUBGItemInstance> ItemInstance;

private:
	friend struct FPUBGEquipmentList;
	friend class UPUBGEquipmentManagerComponent;
	friend class UPUBGItemManagerComponent;

	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType;

	UPROPERTY(NotReplicated)
	TObjectPtr<UPUBGEquipmentManagerComponent> EquipmentManager;

	UPROPERTY(NotReplicated)
	TObjectPtr<APUBGEquipmentBase> SpawnedEquipmentActor;
};

USTRUCT(BlueprintType)
struct FPUBGEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FPUBGEquipmentList() : EquipmentManager(nullptr) {}
	FPUBGEquipmentList(UPUBGEquipmentManagerComponent* InOwnerComponent) : EquipmentManager(InOwnerComponent){}

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

private:
	void Equipment(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance);
	UPUBGItemInstance* Unequipment(EEquipmentSlotType EquipmentSlotType);

public:
	TArray<FPUBGEquipmentEntry>& GetAllEntries() { return Entries; }

private:
	friend class UPUBGEquipmentManagerComponent;
	friend class UPUBGInventoryManagerComponent;
	friend class UPUBGItemManagerComponent;

	UPROPERTY()
	TArray<FPUBGEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	UPUBGEquipmentManagerComponent* EquipmentManager;
};

template<>
struct TStructOpsTypeTraits<FPUBGEquipmentList> : public TStructOpsTypeTraitsBase2<FPUBGEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UPUBGEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ó�� ���ø����̼� �ϱ����� ȣ��Ǵ� �Լ�
	virtual void ReadyForReplication() override;
	// ���ø����̼� ��Ŷ�� �����ϰ� �����ϴ� �Լ� (��Ŷ ���������� ȣ��)
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	void Equipment(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance);
	UPUBGItemInstance* Unequipment(EEquipmentSlotType EquipmentSlotType);
	
	// ����� ���� �� �ٷ� Ȱ��ȭ

public:
	static bool IsWeaponSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsArmorSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsUtilitySlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsFashionSlot(EEquipmentSlotType EquipmentSlotType);
	static bool IsActiveEquipWeaponSlot(EEquipmentSlotType EquipmentSlotType);

	APUBGBaseCharacter* GetCharacter() const;
	AController* GetController() const;
	UPUBGActiveEquipmentManagerComponent* GetActiveEquipmentManager() const;
	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const;

	UPUBGItemInstance* GetItemInstance(EEquipmentSlotType EquipmentSlotType);
	TArray<FPUBGEquipmentEntry>& GetAllEntries();
	FPUBGEquipmentEntry& GetEntry(EEquipmentSlotType EquipmentSlotType);

	// �ش� ��Ʈ���� ���� ���͸� ��ȯ
	APUBGEquipmentBase* GetEntryEquipmentActor(EEquipmentSlotType EquipmentSlotType);

	// �ش� ������ �ν��Ͻ��� ���� ������ ����Ÿ�� ��ȯ
	bool FindEquipmentSlot(UPUBGItemInstance* InItemInstance, EEquipmentSlotType& EquipmentSlotType);

	// ������ ������ �ν��Ͻ� ���� ���� ��ȯ
	bool IsSocketEmpty(EActiveWeaponEquipmentType ActiveEquipmentState);

	// ������� ��� ��� �������� �̵�
	void SetWeaponActiveEquipmentSocketToWeaponEquipmentSocket(EEquipmentSlotType EquipmentSlotType);

	// ��Ȱ��ȭ ���� ��� ���� �����´�.
	FName GetWeaponEquipmentAttachSocket(EEquipmentSlotType EquipmentSlotType) const;
	
	// ���� �÷��̾� ���¿� ���� ���� ���� ����
	void ReplaceWeaponEquipmentSocket();


private:
	UPROPERTY(Replicated)
	FPUBGEquipmentList EquipmentList;
};
