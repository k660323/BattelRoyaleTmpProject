// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "Data/StartUpData/DataAsset_StartUpDataBase.h"
#include "PUBGActiveEquipmentManagerComponent.generated.h"

class UPUBGItemInstance;
class APUBGBaseCharacter;
class APUBGPickupableItemBase;
class APUBGEquipmentBase;
struct FPUBGWeaponAttachInfo;
class UPUBGItemFragment_Weapon;
class UPUBGEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FPUBGActiveEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FPUBGActiveEquipmentEntry() : 
		ItemInstance(nullptr), 
		EquipmentSlotType(EEquipmentSlotType::Count),
		ActiveEquipmentManager(nullptr),
		StartUpDataHandles(),
		BaseStatHandle(),
		GrantTagsHandle(),
		LinkAnimClass(nullptr) {}

private:
	void Init(UPUBGItemInstance* InItemInstance);
	void Equip();
	void Unequip();

public:
	FORCEINLINE UPUBGItemInstance* GetItemInstance() const { return ItemInstance; }
	FORCEINLINE const FActiveGameplayEffectHandle& GetBaseStatHandle() const { return BaseStatHandle; };

private:
	friend struct FPUBGActiveEquipmentList;
	friend class UPUBGActiveEquipmentManagerComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPUBGItemInstance> ItemInstance;

	UPROPERTY(NotReplicated)
	EEquipmentSlotType EquipmentSlotType;

public:
	UPROPERTY(NotReplicated)
	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager;

private:
	UPROPERTY(NotReplicated)
	FPUBGStartUpData_GrantedHandles StartUpDataHandles;

	UPROPERTY(NotReplicated)
	FActiveGameplayEffectHandle BaseStatHandle;

	UPROPERTY(NotReplicated)
	FActiveGameplayEffectHandle GrantTagsHandle;

	UPROPERTY(NotReplicated)
	TSubclassOf<UAnimInstance> LinkAnimClass;
};

USTRUCT(BlueprintType)
struct FPUBGActiveEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FPUBGActiveEquipmentList() : Entries(), ActiveEquipmentManager(nullptr) {}
	FPUBGActiveEquipmentList(UPUBGActiveEquipmentManagerComponent* InOwnerComponent) : Entries(), ActiveEquipmentManager(InOwnerComponent) {}

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

private:
	void Equip(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);

private:
	friend class UPUBGActiveEquipmentManagerComponent;
	
	// ���� ���ø����̼� �迭
	UPROPERTY(VisibleAnywhere)
	TArray<FPUBGActiveEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager;
};

template<>
struct TStructOpsTypeTraits<FPUBGActiveEquipmentList> : public TStructOpsTypeTraitsBase2<FPUBGActiveEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

//USTRUCT(BlueprintType)
//struct FPUBGUseEquipItem
//{
//	GENERATED_BODY()
//
//public:
//	FPUBGUseEquipItem()
//	: UseEquipSlot(EUseEquipSlot::Count), ItemTemplateID(0) {}
//
//	FPUBGUseEquipItem(EUseEquipSlot InUseEquipSlot, int32 InItemTemplateID) 
//	: UseEquipSlot(InUseEquipSlot), ItemTemplateID(InItemTemplateID) {}
//public:
//	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
//	{
//		uint32 uEquipSlotType = (uint32)UseEquipSlot;
//		Ar.SerializeIntPacked(uEquipSlotType);
//		UseEquipSlot = (EUseEquipSlot)uEquipSlotType;
//		
//		uint32 uItemTemplateID = (uint32)ItemTemplateID;
//		Ar.SerializeIntPacked(uItemTemplateID);
//		ItemTemplateID = (int32)uItemTemplateID;
//
//		return bOutSuccess;
//	}
//
//	void Clear()
//	{
//		UseEquipSlot = EUseEquipSlot::Count;
//		ItemTemplateID = 0;
//	}
//
//public:
//	UPROPERTY()
//	EUseEquipSlot UseEquipSlot;
//
//	UPROPERTY()
//	int32 ItemTemplateID;
//};

//template<>
//struct TStructOpsTypeTraits<FPUBGUseEquipItem> : public TStructOpsTypeTraitsBase2<FPUBGUseEquipItem>
//{
//	enum
//	{
//		WithNetSerializer = true
//	};
//};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UPUBGActiveEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGActiveEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ó�� ���ø����̼� �ϱ����� ȣ��Ǵ� �Լ�
	virtual void ReadyForReplication() override;
	// ���ø����̼� ��Ŷ�� �����ϰ� �����ϴ� �Լ� (��Ŷ ���������� ȣ��)
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:	
	void Equip(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance);
	void Unequip(EEquipmentSlotType EquipmentSlotType);
	
	void EquipCurrentSlots();
	void UnequipCurrentSlots();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ChangeEquipState(EActiveWeaponEquipmentType NewActiveEquipmentState);

	UFUNCTION(BlueprintCallable)
	bool CanChangeEquipState(EActiveWeaponEquipmentType NewActiveEquipmentState) const;

private:
	UFUNCTION()
	void OnRep_CurActiveWeaponEquipmentType(EActiveWeaponEquipmentType PrevEquipmentState);

public:
	APUBGBaseCharacter* GetCharacter() const;
	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const;
	UPUBGEquipmentManagerComponent* GetEquipmentManager() const;

	APUBGEquipmentBase* GetEntryEquipmentActorFromEquipmentManager(EWeaponType WeaponType);

	UPUBGItemInstance* GetEquipItemInstance(EEquipmentSlotType EquipmentSlotType);
	UPUBGItemInstance* GetEquipItemInstance(EArmorType ArmorType);
	UPUBGItemInstance* GetEquipItemInstance(EWeaponType WeaponType);
	UPUBGItemInstance* GetEquipItemInstance(EUtilityType UtilityType);
	UPUBGItemInstance* GetEquipItemInstance(EFashionType FashionType);

	TArray<FPUBGActiveEquipmentEntry>& GetAllEntry();
	FPUBGActiveEquipmentEntry& GetEntry(EEquipmentSlotType EquipmentSlotType);
	// ���� Ȱ��ȭ���� ������ ���͸� ��ȯ
	FPUBGActiveEquipmentEntry& GetCurActiveEquipmentEntry();

	// ���� ��� ������ Ȱ��ȭ ���� ��� �������� �̵�
	void SetWeaponEquipmentSocketToWeaponActiveEquipmentSocket(EEquipmentSlotType EquipmentSlotType);

	// ���� �÷��̾� ���¿� ���� ���� ���� ����
	void RefreshCurActiveEuipmentAttachSocket();

public:
	// �ܺο��� Ư�� ó�� �ʿ�
	static EWeaponType EquipmentSlotConvertToWeaponType(EEquipmentSlotType EquipmentSlotType);
	// �ܺο��� Ư�� ó�� �ʿ�
	static EEquipmentSlotType WeaponTypeConvertToEquipmentSlotType(EWeaponType WeaponType);

	static EFashionType EquipmentSlotConvertToFashionType(EEquipmentSlotType EquipmentSlotType);
	static EEquipmentSlotType FashionTypeConvertToEquipmentSlotType(EFashionType FashionType);
	
	static EArmorType EquipmentSlotConvertToArmorType(EEquipmentSlotType EquipmentSlotType);
	static EEquipmentSlotType ArmorTypeConvertToEquipmentSlotType(EArmorType ArmorType);
	
	static EUtilityType EquipmentSlotConvertToUtilityType(EEquipmentSlotType EquipmentSlotType);
	static EEquipmentSlotType UtilityTypeConvertToEquipmentSlotType(EUtilityType UtilityType);

	static EActiveWeaponEquipmentType ConvertToActiveWeaponEquipmentType(EEquipmentSlotType WeaponSlotType);
	static EEquipmentSlotType ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(EActiveWeaponEquipmentType ActiveEquipmentState);

	
	FORCEINLINE EActiveWeaponEquipmentType GetCurActiveWeaponEquipmentType() const { return CurActiveWeaponEquipmentType; }
private:
	friend struct FPUBGActiveEquipmentEntry;

	UPROPERTY(VisibleAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FPUBGActiveEquipmentList ActiveEquipmentList;

	UPROPERTY(ReplicatedUsing = OnRep_CurActiveWeaponEquipmentType)
	EActiveWeaponEquipmentType CurActiveWeaponEquipmentType;
};
