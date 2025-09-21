// Kakanu Portfolio Project


#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "Item/Fragments/PUBGItemFragment_Fashion.h"
#include "Item/Fragments/PUBGItemFragment_Armor.h"
#include "Item/Fragments/PUBGItemFragment_Utility.h"
#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "Item/Fragments/PUBGItemFragment_Weapon_Gun.h"
#include "Item/PUBGItemInstance.h"
#include "Item/PUBGItemTemplate.h"
#include "System/PUBGAssetManager.h"
#include "Engine/ActorChannel.h"
#include "Data/StartUpData/DataAsset_StartUpDataBase.h"
#include "GameMode/PUBGGameMode.h"
#include "PUBGGameplayTags.h"
#include "Animation/AnimInstances/PUBGLinkedAnimLayer.h"
#include "Actors/PUBGEquipmentBase.h"
#include "Data/PUBGItemData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGActiveEquipmentManagerComponent)

// FPUBGActiveEquipmentEntry Start ----------------------------------------

void FPUBGActiveEquipmentEntry::Init(UPUBGItemInstance* InItemInstance)
{
	if (ItemInstance == InItemInstance)
	{
		return;
	}

	ItemInstance = InItemInstance;
	ItemInstance ? Equip() : Unequip();
}

void FPUBGActiveEquipmentEntry::Equip()
{
	APUBGBaseCharacter* Character = ActiveEquipmentManager->GetCharacter();

	if (Character == nullptr)
	{
		return;
	}

	const UPUBGItemFragment_Equipable* EquipableFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Equipable>();
	if (EquipableFragment == nullptr)
	{
		return;
	}

	// 서버 로직
	if (ActiveEquipmentManager->GetOwner()->HasAuthority())
	{
		// 어빌리티, 이펙트, 어트리뷰트 적용
		if (UPUBGAbilitySystemComponent* ASC = ActiveEquipmentManager->GetPUBGAbilitySystemComponent())
		{
			// 현재 소켓에 적용된 어빌리티 제거
			StartUpDataHandles.ClearByAbilitySystem(ASC);

			// 어빌리티 부여
			if (EquipableFragment->BaseAbilitySet)
			{
				// ASC에 부여, 어빌리티 추적, 능력 기원
				EquipableFragment->BaseAbilitySet->GiveToAbilitySystemComponent(ASC, &StartUpDataHandles, ItemInstance);
			}

			// 장비 능력치 이펙트 제거
			ASC->RemoveActiveGameplayEffect(BaseStatHandle);

			// 태그 부여
			if (EquipableFragment->GrantedTags.IsEmpty() == false)
			{
				GrantTagsHandle = ASC->AddDynamicTagsGameplayEffect(EquipableFragment->GrantedTags);
			}

			// 장비 능력치 설정
			if (ItemInstance->GetStatContainer().GetStacks().Num() > 0)
			{
				const  TSubclassOf<UGameplayEffect> AttributeModifierGE = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("Attribute_Modifier"));
				check(AttributeModifierGE);

				const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(*AttributeModifierGE, 1.f, ContextHandle);

				// SetByCaller 값 0으로 초기화
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

	if (EquipableFragment->EquipmentType == EEquipmentType::Weapon)
	{
		const UPUBGItemFragment_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();
		if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			EActiveWeaponEquipmentType ActiveEquipmentState = UPUBGActiveEquipmentManagerComponent::ConvertToActiveWeaponEquipmentType(EquipmentSlotType);
			if (ActiveEquipmentState == ActiveEquipmentManager->CurActiveWeaponEquipmentType)
			{
				if (LinkAnimClass)
				{
					MeshComponent->UnlinkAnimClassLayers(LinkAnimClass);
				}

				LinkAnimClass = AttachmentFragment->AttachmentAnimLayerToLinkClass;

				if (LinkAnimClass)
				{
					MeshComponent->LinkAnimClassLayers(LinkAnimClass);
				}
			}
			else
			{
				ensureMsgf(false, TEXT("ActiveEquipmentState Not Equal CurActiveWeaponEquipmentType"));
			}
		}
	}
}

void FPUBGActiveEquipmentEntry::Unequip()
{
	APUBGBaseCharacter* Character = ActiveEquipmentManager->GetCharacter();

	if (Character == nullptr)
	{
		return;
	}

	if (ActiveEquipmentManager->GetOwner()->HasAuthority())
	{
		// 어빌리티, 이펙트, 어트리뷰트 적용
		if (UPUBGAbilitySystemComponent* ASC = ActiveEquipmentManager->GetPUBGAbilitySystemComponent())
		{
			// 현재 소켓에 적용된 어빌리티 제거
			StartUpDataHandles.ClearByAbilitySystem(ASC);
			// 장비 능력치 이펙트 제거
			ASC->RemoveActiveGameplayEffect(BaseStatHandle);
			BaseStatHandle.Invalidate();
			// 태그 제거
			ASC->RemoveActiveGameplayEffect(GrantTagsHandle);
			GrantTagsHandle.Invalidate();
		}
	}

	if (UPUBGEquipmentManagerComponent::IsWeaponSlot(EquipmentSlotType))
	{
		if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			EActiveWeaponEquipmentType ActiveEquipmentState = UPUBGActiveEquipmentManagerComponent::ConvertToActiveWeaponEquipmentType(EquipmentSlotType);
			
			if (ActiveEquipmentManager->GetOwner()->HasAuthority())
			{
				if (ActiveEquipmentState != ActiveEquipmentManager->CurActiveWeaponEquipmentType)
				{
					MeshComponent->UnlinkAnimClassLayers(LinkAnimClass);
					LinkAnimClass = nullptr;
				}
			}
			else
			{
				if (ActiveEquipmentState != ActiveEquipmentManager->CurActiveWeaponEquipmentType)
				{
					if (ActiveEquipmentManager->CurActiveWeaponEquipmentType != EActiveWeaponEquipmentType::Count)
					{
						FPUBGActiveEquipmentEntry& Entry = ActiveEquipmentManager->GetCurActiveEquipmentEntry();
						if (Entry.LinkAnimClass != LinkAnimClass)
						{
							MeshComponent->UnlinkAnimClassLayers(LinkAnimClass);
							LinkAnimClass = nullptr;
						}
					}
					else
					{
						MeshComponent->UnlinkAnimClassLayers(LinkAnimClass);
						LinkAnimClass = nullptr;
					}
				}
			}
		}
	}
}

// FPUBGActiveEquipmentEntry End ----------------------------------------


// FPUBGActiveEquipmentList Start ----------------------------------------
bool FPUBGActiveEquipmentList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FPUBGActiveEquipmentEntry, FPUBGActiveEquipmentList>(Entries, DeltaParams, *this);
}

void FPUBGActiveEquipmentList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 AddedIndex : AddedIndices)
	{
		// 비 레플리케이션 변수 수동 할당
		FPUBGActiveEquipmentEntry& Entry = Entries[AddedIndex];
		Entry.ActiveEquipmentManager = ActiveEquipmentManager;
		Entry.EquipmentSlotType = (EEquipmentSlotType)AddedIndex;

		// 레플리케이션 변수
		if (Entry.GetItemInstance())
		{
			Entry.Equip();
		}
	}
}

void FPUBGActiveEquipmentList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 ChangedIndex : ChangedIndices)
	{
		// 비 레플리케이션 변수 수동 할당
		FPUBGActiveEquipmentEntry& Entry = Entries[ChangedIndex];
		// 레플리케이션 변수
		Entry.GetItemInstance() ? Entry.Equip() : Entry.Unequip();
	}
}

void FPUBGActiveEquipmentList::Equip(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance)
{
	FPUBGActiveEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Init(ItemInstance);
	MarkItemDirty(Entry);
}

void FPUBGActiveEquipmentList::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	FPUBGActiveEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	Entry.Init(nullptr);
	MarkItemDirty(Entry);
}

// FPUBGActiveEquipmentList End ----------------------------------------

// Sets default values for this component's properties
UPUBGActiveEquipmentManagerComponent::UPUBGActiveEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ActiveEquipmentList(this)
	, CurActiveWeaponEquipmentType(EActiveWeaponEquipmentType::Count)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UPUBGActiveEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 서버만 실행
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TArray<FPUBGActiveEquipmentEntry>& Entries = ActiveEquipmentList.Entries;

		Entries.SetNum((int32)EEquipmentSlotType::Count);

		for (int32 i = 0; i < Entries.Num(); i++)
		{
			FPUBGActiveEquipmentEntry& Entry = Entries[i];
			Entry.ActiveEquipmentManager = this;
			Entry.EquipmentSlotType = (EEquipmentSlotType)i;
			// 배열 개수 바뀐거 알려줌
			ActiveEquipmentList.MarkItemDirty(Entry);
		}
	}
}

void UPUBGActiveEquipmentManagerComponent::UninitializeComponent()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// 장비 해제
		TArray<FPUBGActiveEquipmentEntry>& Entries = ActiveEquipmentList.Entries;
		for (int32 i = 0; i < Entries.Num(); i++)
		{
			Unequip((EEquipmentSlotType)i);
		}
	}


	Super::UninitializeComponent();

}

void UPUBGActiveEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ActiveEquipmentList);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurActiveWeaponEquipmentType, COND_None, REPNOTIFY_OnChanged);
	
	// DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentUseEquipItem, COND_None, REPNOTIFY_OnChanged);
}

void UPUBGActiveEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPUBGActiveEquipmentEntry& Entry : ActiveEquipmentList.Entries)
		{
			UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();
			if (IsValid(ItemInstance))
			{
				AddReplicatedSubObject(ItemInstance);
			}
		}
	}

}

bool UPUBGActiveEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FPUBGActiveEquipmentEntry& Entry : ActiveEquipmentList.Entries)
	{
		UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();

		if (IsValid(ItemInstance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UPUBGActiveEquipmentManagerComponent::Equip(EEquipmentSlotType EquipmentSlotType, UPUBGItemInstance* ItemInstance)
{
	check(GetOwner()->HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count || ItemInstance == nullptr)
	{
		return;
	}

	ActiveEquipmentList.Equip(EquipmentSlotType, ItemInstance);

	// UObject는 FFastArraySerializer에서 자동으로 레플리케이션 등록해주지 않아서 수동 등록
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		// 1회 호출 한번만 복제
		AddReplicatedSubObject(ItemInstance);
	}
}

void UPUBGActiveEquipmentManagerComponent::Unequip(EEquipmentSlotType EquipmentSlotType)
{
	check(GetOwner()->HasAuthority());

	if (EquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	TArray<FPUBGActiveEquipmentEntry>& Entries = ActiveEquipmentList.Entries;
	FPUBGActiveEquipmentEntry& Entry = Entries[(int32)EquipmentSlotType];
	UPUBGItemInstance* RemovedItemInstance = Entry.GetItemInstance();

	if (RemovedItemInstance == nullptr)
	{
		return;
	}

	ActiveEquipmentList.Unequip(EquipmentSlotType);

	// 강제 레플리케이션 
	// Iris를 쓰면 효율적인 레플리케이션 타이밍을 제어할수 있다. (중복 제거 및 빠른 레플리케이션 타이밍)
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && RemovedItemInstance)
	{
		RemoveReplicatedSubObject(RemovedItemInstance);
	}
}

void UPUBGActiveEquipmentManagerComponent::EquipCurrentSlots()
{
	check(GetOwner()->HasAuthority());

	if (CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Count)
	{
		return;
	}

	if (UPUBGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		EEquipmentSlotType EquipmentSlotType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
		Equip(EquipmentSlotType, EquipmentManager->GetItemInstance(EquipmentSlotType));

		// 손잡이에 장착
		SetWeaponEquipmentSocketToWeaponActiveEquipmentSocket(EquipmentSlotType);
	}
}

void UPUBGActiveEquipmentManagerComponent::UnequipCurrentSlots()
{
	check(GetOwner()->HasAuthority());

	if (CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Count)
	{
		return;
	}

	EEquipmentSlotType EquipmentSlotType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
	Unequip(EquipmentSlotType);

	if (UPUBGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		EquipmentManager->SetWeaponActiveEquipmentSocketToWeaponEquipmentSocket(EquipmentSlotType);
	}
}

void UPUBGActiveEquipmentManagerComponent::ChangeEquipState(EActiveWeaponEquipmentType NewActiveEquipmentState)
{
	check(GetOwner()->HasAuthority());

	if (CanChangeEquipState(NewActiveEquipmentState))
	{
		// 같은 장착 무기 타입이면 무장해제
		if (CurActiveWeaponEquipmentType == NewActiveEquipmentState)
		{
			// Unarmed의 ItemInstance 는 nullptr
			NewActiveEquipmentState = EActiveWeaponEquipmentType::Unarmed;
		}

		UnequipCurrentSlots();
		CurActiveWeaponEquipmentType = NewActiveEquipmentState;
		EquipCurrentSlots();
	}
}

bool UPUBGActiveEquipmentManagerComponent::CanChangeEquipState(EActiveWeaponEquipmentType NewActiveEquipmentState) const
{
	if (NewActiveEquipmentState == EActiveWeaponEquipmentType::Count)
	{
		return false;
	}

	if (CurActiveWeaponEquipmentType == EActiveWeaponEquipmentType::Unarmed && NewActiveEquipmentState == EActiveWeaponEquipmentType::Unarmed)
	{
		return false;
	}

	if (CurActiveWeaponEquipmentType == NewActiveEquipmentState)
	{
		return true;
	}

	UPUBGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager();
	if (EquipmentManager == nullptr)
	{
		return false;
	}

	// 장착할 무기가 존재해야한다.
	return EquipmentManager->IsSocketEmpty(NewActiveEquipmentState) == false;
}

void UPUBGActiveEquipmentManagerComponent::OnRep_CurActiveWeaponEquipmentType(EActiveWeaponEquipmentType PrevEquipmentState)
{
	// 브로드 캐스트

}

//void UPUBGActiveEquipmentManagerComponent::OnRep_LinkAnimInstance(TSubclassOf<UAnimInstance> PreLinkAnimInstance)
//{
//	APUBGBaseCharacter* Character = GetCharacter();
//
//	if (Character == nullptr)
//	{
//		return;
//	}
//
//	if (USkeletalMeshComponent* MeshComponent = Character->GetMesh())
//	{
//		if (PreLinkAnimInstance != nullptr)
//		{
//			UAnimInstance* PreAnimInstanceCDO = PreLinkAnimInstance.GetDefaultObject();
//			MeshComponent->LinkAnimClassLayers(PreLinkAnimInstance);
//		}
//
//		if (LinkAnimInstanceClass != nullptr)
//		{
//			if (CurActiveWeaponEquipmentType != EActiveWeaponEquipmentType::Unarmed && CurActiveWeaponEquipmentType != EActiveWeaponEquipmentType::Count)
//			{
//				MeshComponent->LinkAnimClassLayers(LinkAnimInstanceClass);
//			}
//		}
//	}
//}

APUBGBaseCharacter* UPUBGActiveEquipmentManagerComponent::GetCharacter() const
{
	return Cast<APUBGBaseCharacter>(GetOwner());
}

UPUBGAbilitySystemComponent* UPUBGActiveEquipmentManagerComponent::GetPUBGAbilitySystemComponent() const
{
	return Cast<UPUBGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

UPUBGEquipmentManagerComponent* UPUBGActiveEquipmentManagerComponent::GetEquipmentManager() const
{
	if (APUBGBaseCharacter* Character = GetCharacter())
	{
		return Character->FindComponentByClass<UPUBGEquipmentManagerComponent>();
	}

	return nullptr;
}

APUBGEquipmentBase* UPUBGActiveEquipmentManagerComponent::GetEntryEquipmentActorFromEquipmentManager(EWeaponType WeaponType)
{
	UPUBGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager();

	if (EquipmentManager == nullptr)
	{
		return nullptr;
	}

	if (WeaponType == EWeaponType::Count)
	{
		EEquipmentSlotType EquipmentSlotWeaponType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
		return EquipmentManager->GetEntryEquipmentActor(EquipmentSlotWeaponType);
	}

	EEquipmentSlotType EquipmentSlotType = WeaponTypeConvertToEquipmentSlotType(WeaponType);

	if (EquipmentSlotType == EEquipmentSlotType::Main)
	{
		EquipmentSlotType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
	}

	return EquipmentManager->GetEntryEquipmentActor(EquipmentSlotType);
}


UPUBGItemInstance* UPUBGActiveEquipmentManagerComponent::GetEquipItemInstance(EEquipmentSlotType EquipmentSlotType)
{
	if (EquipmentSlotType < EEquipmentSlotType::Unarmed || EquipmentSlotType >= EEquipmentSlotType::Count)
	{
		return nullptr;
	}

	return ActiveEquipmentList.Entries[(int32)EquipmentSlotType].ItemInstance;
}

UPUBGItemInstance* UPUBGActiveEquipmentManagerComponent::GetEquipItemInstance(EArmorType ArmorType)
{
	EEquipmentSlotType EquipmentSlotType = ArmorTypeConvertToEquipmentSlotType(ArmorType);
	
	return GetEquipItemInstance(EquipmentSlotType);
}

UPUBGItemInstance* UPUBGActiveEquipmentManagerComponent::GetEquipItemInstance(EWeaponType WeaponType)
{
	if (WeaponType == EWeaponType::Count)
	{
		EEquipmentSlotType EquipmentSlotWeaponType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
		return GetEquipItemInstance(EquipmentSlotWeaponType);
	}

	EEquipmentSlotType EquipmentSlotType = WeaponTypeConvertToEquipmentSlotType(WeaponType);

	if (EquipmentSlotType == EEquipmentSlotType::Main)
	{
		EquipmentSlotType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
	}

	return GetEquipItemInstance(EquipmentSlotType);
}

UPUBGItemInstance* UPUBGActiveEquipmentManagerComponent::GetEquipItemInstance(EUtilityType UtilityType)
{
	EEquipmentSlotType EquipmentSlotType = UtilityTypeConvertToEquipmentSlotType(UtilityType);

	return GetEquipItemInstance(EquipmentSlotType);
}

UPUBGItemInstance* UPUBGActiveEquipmentManagerComponent::GetEquipItemInstance(EFashionType FashionType)
{
	EEquipmentSlotType EquipmentSlotType = FashionTypeConvertToEquipmentSlotType(FashionType);

	return GetEquipItemInstance(EquipmentSlotType);
}

TArray<FPUBGActiveEquipmentEntry>& UPUBGActiveEquipmentManagerComponent::GetAllEntry()
{
	return ActiveEquipmentList.Entries;
}

FPUBGActiveEquipmentEntry& UPUBGActiveEquipmentManagerComponent::GetEntry(EEquipmentSlotType EquipmentSlotType)
{
	return ActiveEquipmentList.Entries[(int32)EquipmentSlotType];
}


FPUBGActiveEquipmentEntry& UPUBGActiveEquipmentManagerComponent::GetCurActiveEquipmentEntry()
{
	EEquipmentSlotType EquipmentSlotType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
	FPUBGActiveEquipmentEntry& Entry = GetEntry(EquipmentSlotType);
	return Entry;
}

void UPUBGActiveEquipmentManagerComponent::SetWeaponEquipmentSocketToWeaponActiveEquipmentSocket(EEquipmentSlotType EquipmentSlotType)
{
	if (UPUBGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponent();
		if (ASC == nullptr)
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

		FPUBGEquipmentEntry& Entry = EquipmentManager->GetEntry(EquipmentSlotType);
		UPUBGItemInstance* ItemInstance = Entry.GetItemInstance();
		if (ItemInstance == nullptr)
		{
			return;
		}

		APUBGEquipmentBase* SpawnWeaponActor = Entry.GetEquipmentActor();
		const UPUBGItemFragment_Attachment* AttachmentFragment = ItemInstance->FindFragmentByClass<UPUBGItemFragment_Attachment>();

		if (SpawnWeaponActor && AttachmentFragment)
		{
			FName AttachSocket = AttachmentFragment->GetActiveEquipmentAttachSocket(ASC);
			FTransform NewTransform = MeshComponent->GetSocketTransform(AttachSocket);
			SpawnWeaponActor->SetActorTransform(NewTransform);
			FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, true);
			SpawnWeaponActor->AttachToComponent(MeshComponent, AttachmentTransformRules, AttachSocket);
			SpawnWeaponActor->SetActorRelativeTransform(AttachmentFragment->AttachInfo.AttachTransformOffset);
		}
	}
}

void UPUBGActiveEquipmentManagerComponent::RefreshCurActiveEuipmentAttachSocket()
{
	EEquipmentSlotType EquipmentSlotType = ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(CurActiveWeaponEquipmentType);
	if (EquipmentSlotType == EEquipmentSlotType::Unarmed || EquipmentSlotType == EEquipmentSlotType::Count)
	{
		return;
	}

	SetWeaponEquipmentSocketToWeaponActiveEquipmentSocket(EquipmentSlotType);
}

EWeaponType UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToWeaponType(EEquipmentSlotType EquipmentSlotType)
{
	EWeaponType WeaponType = EWeaponType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Unarmed:
		WeaponType = EWeaponType::Unarmed;
		break;
	case EEquipmentSlotType::Main:
	case EEquipmentSlotType::Main2:
		WeaponType = EWeaponType::Main; // 첫번째 메인소켓 반환 (후속 처리 필요)
		break;
	case EEquipmentSlotType::Sub:
		WeaponType = EWeaponType::Sub;
		break;
	case EEquipmentSlotType::Etc:
		WeaponType = EWeaponType::ETC;
		break;
	case EEquipmentSlotType::Throw:
		WeaponType = EWeaponType::Throw;
		break;
	}

	return WeaponType;
}

EEquipmentSlotType UPUBGActiveEquipmentManagerComponent::WeaponTypeConvertToEquipmentSlotType(EWeaponType WeaponType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (WeaponType)
	{
	case EWeaponType::Unarmed:
		EquipmentSlotType = EEquipmentSlotType::Unarmed;
		break;
	case EWeaponType::Main:
		// 첫번째 메인소켓 반환 (후속 처리 필요)
		EquipmentSlotType = EEquipmentSlotType::Main;
		break;
	case EWeaponType::Sub:
		EquipmentSlotType = EEquipmentSlotType::Sub;
		break;
	case EWeaponType::ETC:
		EquipmentSlotType = EEquipmentSlotType::Etc;
		break;
	case EWeaponType::Throw:
		EquipmentSlotType = EEquipmentSlotType::Throw;
		break;
	}

	return EquipmentSlotType;
}

EFashionType UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToFashionType(EEquipmentSlotType EquipmentSlotType)
{
	EFashionType FashionType = EFashionType::Count;

	switch (EquipmentSlotType)
	{
	case EEquipmentSlotType::Cap:
		FashionType = EFashionType::Cap;
		break;
	case EEquipmentSlotType::ClothTop:
		FashionType = EFashionType::ClothTop;
		break;
	case EEquipmentSlotType::ClothBottom:
		FashionType = EFashionType::ClothBottom;
		break;
	case EEquipmentSlotType::Whole:
		FashionType = EFashionType::Whole;
		break;
	case EEquipmentSlotType::Shoes:
		FashionType = EFashionType::Shoes;
		break;
	}

	return FashionType;
}

EEquipmentSlotType UPUBGActiveEquipmentManagerComponent::FashionTypeConvertToEquipmentSlotType(EFashionType FashionType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (FashionType)
	{
	case EFashionType::Cap:
		EquipmentSlotType = EEquipmentSlotType::Cap;
		break;
	case EFashionType::ClothTop:
		EquipmentSlotType = EEquipmentSlotType::ClothTop;
		break;
	case EFashionType::ClothBottom:
		EquipmentSlotType = EEquipmentSlotType::ClothBottom;
		break;
	case EFashionType::Whole:
		EquipmentSlotType = EEquipmentSlotType::Whole;
		break;
	case EFashionType::Shoes:
		EquipmentSlotType = EEquipmentSlotType::Shoes;
		break;
	}

	return EquipmentSlotType;
}

EArmorType UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToArmorType(EEquipmentSlotType EquipmentSlotType)
{
	EArmorType ArmorType = EArmorType::Count;

	switch (EquipmentSlotType)
	{
	
	case EEquipmentSlotType::Helmet:
		ArmorType = EArmorType::Helmet;
		break;
	case EEquipmentSlotType::Vest:
		ArmorType = EArmorType::Vest;
		break;
	}

	return ArmorType;
}

EEquipmentSlotType UPUBGActiveEquipmentManagerComponent::ArmorTypeConvertToEquipmentSlotType(EArmorType ArmorType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;
	switch (ArmorType)
	{
	case EArmorType::Helmet:
		EquipmentSlotType = EEquipmentSlotType::Helmet;
		break;
	case EArmorType::Vest:
		EquipmentSlotType = EEquipmentSlotType::Vest;
		break;
	}

	return EquipmentSlotType;
}

EUtilityType UPUBGActiveEquipmentManagerComponent::EquipmentSlotConvertToUtilityType(EEquipmentSlotType EquipmentSlotType)
{
	EUtilityType UtilityType = EUtilityType::Count;

	switch (EquipmentSlotType)
	{
	
	case EEquipmentSlotType::Belt:
		UtilityType = EUtilityType::Belt;
		break;
	case EEquipmentSlotType::Backpack:
		UtilityType = EUtilityType::Backpack;
		break;
	}

	return UtilityType;
}

EEquipmentSlotType UPUBGActiveEquipmentManagerComponent::UtilityTypeConvertToEquipmentSlotType(EUtilityType UtilityType)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (UtilityType)
	{
	case EUtilityType::Belt:
		EquipmentSlotType = EEquipmentSlotType::Belt;
		break;
	case EUtilityType::Backpack:
		EquipmentSlotType = EEquipmentSlotType::Backpack;
		break;
	}

	return EquipmentSlotType;
}

EActiveWeaponEquipmentType UPUBGActiveEquipmentManagerComponent::ConvertToActiveWeaponEquipmentType(EEquipmentSlotType WeaponSlotType)
{
	EActiveWeaponEquipmentType UseEquipSocket = EActiveWeaponEquipmentType::Count;

	switch (WeaponSlotType)
	{
	case EEquipmentSlotType::Unarmed:
		UseEquipSocket = EActiveWeaponEquipmentType::Unarmed;
		break;
	case EEquipmentSlotType::Main:
		UseEquipSocket = EActiveWeaponEquipmentType::Main;
		break;
	case EEquipmentSlotType::Main2:
		UseEquipSocket = EActiveWeaponEquipmentType::Main2;
		break;
	case EEquipmentSlotType::Sub:
		UseEquipSocket = EActiveWeaponEquipmentType::Sub;
		break;
	case EEquipmentSlotType::Etc:
		UseEquipSocket = EActiveWeaponEquipmentType::Etc;
		break;
	case EEquipmentSlotType::Throw:
		UseEquipSocket = EActiveWeaponEquipmentType::Throw;
		break;
	}

	return UseEquipSocket;
}

EEquipmentSlotType UPUBGActiveEquipmentManagerComponent::ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(EActiveWeaponEquipmentType ActiveEquipmentState)
{
	EEquipmentSlotType EquipmentSlotType = EEquipmentSlotType::Count;

	switch (ActiveEquipmentState)
	{
	case EActiveWeaponEquipmentType::Unarmed:
		EquipmentSlotType = EEquipmentSlotType::Unarmed;
		break;
	case EActiveWeaponEquipmentType::Main:
		EquipmentSlotType = EEquipmentSlotType::Main;
		break;
	case EActiveWeaponEquipmentType::Main2:
		EquipmentSlotType = EEquipmentSlotType::Main2;
		break;
	case EActiveWeaponEquipmentType::Sub:
		EquipmentSlotType = EEquipmentSlotType::Sub;
		break;
	case EActiveWeaponEquipmentType::Etc:
		EquipmentSlotType = EEquipmentSlotType::Etc;
		break;
	case EActiveWeaponEquipmentType::Throw:
		EquipmentSlotType = EEquipmentSlotType::Throw;
		break;
	}

	return EquipmentSlotType;
}