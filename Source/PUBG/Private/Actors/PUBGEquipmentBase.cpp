// Kakanu Portfolio Project


#include "Actors/PUBGEquipmentBase.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Item/Managers/PUBGWeaponAccManagerComponent.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Data/PUBGItemData.h"
#include "Item/PUBGItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGEquipmentBase)

// Sets default values
APUBGEquipmentBase::APUBGEquipmentBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ItemTemplateID(0),
	EquipmentSlotType(EEquipmentSlotType::Count)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	SetRootComponent(ArrowComponent);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SkeletalMeshComponent->SetCollisionProfileName("Weapon");
	SkeletalMeshComponent->SetGenerateOverlapEvents(false);
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
	SkeletalMeshComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
	TraceDebugCollision->SetupAttachment(GetRootComponent());
	TraceDebugCollision->PrimaryComponentTick.bStartWithTickEnabled = false;

	CreateDefaultSubobject<UPUBGWeaponAccManagerComponent>("WeaponAccManager");
}

// Called when the game starts or when spawned
void APUBGEquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		OnRep_EquipmentSlotType();
	}
}

void APUBGEquipmentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemTemplateID);
	DOREPLIFETIME(ThisClass, EquipmentSlotType);
	
}

void APUBGEquipmentBase::Init(int32 InItemTemplateID, EEquipmentSlotType InEquipmentSlotType)
{
	ItemTemplateID = InItemTemplateID;
	EquipmentSlotType = InEquipmentSlotType;
}

bool APUBGEquipmentBase::IsEquipableWeaponAccTypeSocket(EGunAccType WeaponAccType)
{
	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);

	const UPUBGItemFragment_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Weapon>();
	
	if (WeaponFragment->UseAccMap.Contains(WeaponAccType))
	{
		return WeaponFragment->UseAccMap[WeaponAccType];
	}

	return false;
}

bool APUBGEquipmentBase::IsValidWeaponAcc(EGunAccType WeaponAccType)
{
	if (UPUBGWeaponAccManagerComponent* WeaponAccManager = FindComponentByClass<UPUBGWeaponAccManagerComponent>())
	{
		UPUBGItemInstance* ItemInstance = WeaponAccManager->GetWeaponAccItemInstance(WeaponAccType);

		return IsValid(ItemInstance);
	}

	return false;
}

UPUBGItemInstance* APUBGEquipmentBase::GetWeaponAccItemInstance(EGunAccType WeaponAccType)
{
	if (UPUBGWeaponAccManagerComponent* WeaponAccManager = FindComponentByClass<UPUBGWeaponAccManagerComponent>())
	{
		UPUBGItemInstance* ItemInstance = WeaponAccManager->GetWeaponAccItemInstance(WeaponAccType);

		return ItemInstance;
	}

	return nullptr;
}

bool APUBGEquipmentBase::EquipmentWeaponAcc(EGunAccType WeaponAccType, UPUBGItemInstance* InItemInstance)
{
	if (UPUBGWeaponAccManagerComponent* WeaponAccManager = FindComponentByClass<UPUBGWeaponAccManagerComponent>())
	{
		WeaponAccManager->EquipAcc(WeaponAccType, InItemInstance);
		return true;
	}

	return false;
}

UPUBGItemInstance* APUBGEquipmentBase::UnEquipmentWeaponAcc(EGunAccType WeaponAccType)
{
	if (UPUBGWeaponAccManagerComponent* WeaponAccManager = FindComponentByClass<UPUBGWeaponAccManagerComponent>())
	{
		return WeaponAccManager->UnEquipAcc(WeaponAccType);
	}

	return nullptr;
}

UAbilitySystemComponent* APUBGEquipmentBase::GetAbilitySystemComponent() const
{
	UAbilitySystemComponent* ASC = nullptr;
	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	}

	return ASC;
}

EEquipmentSlotType APUBGEquipmentBase::GetEquipmentSlotType() const
{
	return EquipmentSlotType;
}

int32 APUBGEquipmentBase::GetItemTempalteID() const
{
	return ItemTemplateID;
}

EWeaponClassType APUBGEquipmentBase::GetWeaponClassType() const
{
	EWeaponClassType WeaponClassType = EWeaponClassType::Count;

	if (ItemTemplateID == 0)
	{
		return WeaponClassType;
	}

	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);

	if (const UPUBGItemFragment_Weapon* WeaponFragment = ItemTemplate.FindFragmentByClass<UPUBGItemFragment_Weapon>())
	{
		WeaponClassType = WeaponFragment->WeaponClassType;
	}
	
	return WeaponClassType;
}

USkeletalMeshComponent* APUBGEquipmentBase::GetEquipmentMesh() const
{
	return SkeletalMeshComponent;
}

UBoxComponent* APUBGEquipmentBase::GetTraceDebugCollision() const
{
	return TraceDebugCollision;
}

void APUBGEquipmentBase::OnRep_EquipmentSlotType()
{
	AActor* OwnerActor = GetOwner();
	UPUBGEquipmentManagerComponent* EquipmentManager = nullptr;

	if (OwnerActor)
	{
		EquipmentManager = OwnerActor->FindComponentByClass<UPUBGEquipmentManagerComponent>();

		if(EquipmentManager)
		{
			TArray<FPUBGEquipmentEntry>& Entries = EquipmentManager->GetAllEntries();
			Entries[(int32)EquipmentSlotType].SetEquipmentActor(this);
		}
	}
	
	if(OwnerActor == nullptr || EquipmentManager == nullptr)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_EquipmentSlotType);
	}
}
