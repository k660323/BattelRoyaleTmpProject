// Kakanu Portfolio Project


#include "Actors/PUBGPickupableItemBase.h"
#include "Item/PUBGItemTemplate.h"
#include "Item/PUBGItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Data/PUBGItemData.h"
#include "System/PUBGAssetManager.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPickupableItemBase)

// Sets default values
APUBGPickupableItemBase::APUBGPickupableItemBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName("NoCollision");
	MeshComponent->SetupAttachment(RootComponent);
	
	PickupCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupCollision"));
	PickupCollision->SetCollisionProfileName("Pickupable");
	PickupCollision->SetupAttachment(RootComponent);

	bAlwaysRelevant = false;
	// Ŭ�󿡼� ���������� FadeInOut ó�� �ϴ°� ������ 
	// �߰� �ӵ��� �������� ���� �Ǵ� �ӵ��� ����
	NetCullDistanceSquared = FMath::Square(3000.0f);
	CullingDistanceSquared = FMath::Square(2500.0f);
	NetUpdateFrequency = 1.0f;
	MinNetUpdateFrequency = 0.0f;
	bOnlyRelevantToOwner = false;

	InteractionInfo.Title = FText::FromString(TEXT("Pick Up"));
}

void APUBGPickupableItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(CullingDistanceTimerHandle, this, &ThisClass::DistanceCulling, 0.5f, true, 0.0f);
	}
}

void APUBGPickupableItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority() == false && CullingDistanceTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CullingDistanceTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void APUBGPickupableItemBase::DistanceCulling()
{
	// �ڱ� �ڽ��� ���� ��ȯ (Ŭ��)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (PlayerPawn)
	{
		float Distance = (PlayerPawn->GetActorLocation() - GetActorLocation()).SizeSquared();

		// ���̰� ����
		if (Distance <= CullingDistanceSquared)
		{
			SetActorHiddenInGame(false);
		}
		// �Ⱥ��̰� ����
		else
		{
			SetActorHiddenInGame(true);
		}
	}

}

void APUBGPickupableItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PickupInfo, COND_None, REPNOTIFY_OnChanged);
}

void APUBGPickupableItemBase::SetPickupInfo(const FPUBGPickupInstance& InPickupInfo)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (InPickupInfo.ItemInstance)
	{
		PickupInfo = InPickupInfo;
		OnRep_PickupInfo();
	}
	else
	{
		ForceNetUpdate();
		Destroy();
	}
}

void APUBGPickupableItemBase::GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const
{
	OutMeshComponents.Add(MeshComponent);
}

UBoxComponent* APUBGPickupableItemBase::GetCollisionBoxComponent() const
{
	return PickupCollision;
}

void APUBGPickupableItemBase::OnRep_PickupInfo()
{
	if (const UPUBGItemInstance* ItemInstance = PickupInfo.ItemInstance)
	{
		const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemInstance->GetItemTemplateID());
		// ��ȣ �ۿ�
		InteractionInfo.Content = ItemTemplate.DisplayName;

		// ������ �Ž�
		if (IsNetMode(NM_DedicatedServer) == false)
		{
			TSoftObjectPtr<UStaticMesh> PickupableMeshPath = ItemTemplate.PickupableMesh;
			if (PickupableMeshPath.IsNull() == false)
			{
				if (UStaticMesh* PickupableMesh = UPUBGAssetManager::GetAssetByPath(PickupableMeshPath))
				{
					MeshComponent->SetStaticMesh(PickupableMesh);
				}
			}
		}
		
		// �Ž� �� �浹 ���� ��ġ ����
		MeshComponent->SetRelativeTransform(ItemTemplate.PickupableMeshOffset);
		PickupCollision->SetRelativeTransform(ItemTemplate.PickupableCollisionBoxOffset);
		PickupCollision->SetBoxExtent(ItemTemplate.PickupableBoxExtentOffset);
	}
}

