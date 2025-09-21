// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PUBGWeaponAccManagerComponent.generated.h"

class UPUBGItemInstance;
class UPUBGAbilitySystemComponent;
class APUBGBaseCharacter;
class APUBGEquipmentBase;

USTRUCT(BlueprintType)
struct FPUBGWeaponAccEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FPUBGWeaponAccEntry() : 
		ItemInstance(nullptr),
		WeaponAccType(EGunAccType::Count),
		WeaponAccMesh(nullptr),
		WeaponAccManager(nullptr)
		{}

	void Init(UPUBGItemInstance* InItemInstance);
	UPUBGItemInstance* Reset();

	void Equip();
	void UnEquip();

	FORCEINLINE UPUBGItemInstance* GetItemInstance() const { return ItemInstance; }

private:
	friend class UPUBGWeaponAccManagerComponent;
	friend struct FPUBGWeaponAccList;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPUBGItemInstance> ItemInstance;

	UPROPERTY(NotReplicated)
	FActiveGameplayEffectHandle BaseStatHandle;

	UPROPERTY(NotReplicated)
	FActiveGameplayEffectHandle GrantTagsHandle;

	UPROPERTY(NotReplicated)
	EGunAccType WeaponAccType;

	UPROPERTY(NotReplicated)
	TObjectPtr<UStaticMeshComponent> WeaponAccMesh;

public:
	UPROPERTY(NotReplicated)
	TObjectPtr<UPUBGWeaponAccManagerComponent> WeaponAccManager;
};

USTRUCT(BlueprintType)
struct FPUBGWeaponAccList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FPUBGWeaponAccList() : WeaponAccManager(nullptr) {}
	FPUBGWeaponAccList(UPUBGWeaponAccManagerComponent* InWeaponAccManager) : WeaponAccManager(InWeaponAccManager) {}

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

private:
	void EquipAcc(EGunAccType AccType, UPUBGItemInstance* InItemInstance);
	UPUBGItemInstance* UnEquipAcc(EGunAccType AccType);

private:
	friend class UPUBGWeaponAccManagerComponent;

	UPROPERTY(VisibleAnywhere)
	TArray<FPUBGWeaponAccEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UPUBGWeaponAccManagerComponent> WeaponAccManager;
};

template<>
struct TStructOpsTypeTraits<FPUBGWeaponAccList> : public TStructOpsTypeTraitsBase2<FPUBGWeaponAccList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UPUBGWeaponAccManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGWeaponAccManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 처음 레플리케이션 하기전에 호출되는 함수
	virtual void ReadyForReplication() override;
	// 레플리케이션 패킷을 세밀하게 제어하는 함수 (패킷 보낼때마다 호출)
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	void EquipAcc(EGunAccType AccType, UPUBGItemInstance* InItemInstance);
	UPUBGItemInstance* UnEquipAcc(EGunAccType AccType);

public:
	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const;

	UPUBGItemInstance* GetWeaponAccItemInstance(EGunAccType WeaponAccType);

	TArray<FPUBGWeaponAccEntry>& GetAllEntry();
	FPUBGWeaponAccEntry& GetEntry(EGunAccType WeaponAccType);

	APUBGEquipmentBase* GetOwnerAsEquipment() const;
	APUBGBaseCharacter* GetOwnerAsCharacter() const;

private:
	UPROPERTY(VisibleAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FPUBGWeaponAccList WeaponAccList;
};
