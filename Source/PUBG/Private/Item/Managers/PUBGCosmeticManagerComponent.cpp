// Kakanu Portfolio Project


#include "Item/Managers/PUBGCosmeticManagerComponent.h"
#include "GameFramework/Character.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGCharacterDefaultFashion.h"
#include "Characters/PUBGBaseCharacter.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGCosmeticManagerComponent)

// Sets default values for this component's properties
UPUBGCosmeticManagerComponent::UPUBGCosmeticManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UPUBGCosmeticManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitSkeletalMesh();
}

void UPUBGCosmeticManagerComponent::InitSkeletalMesh()
{
	if (bInitializedSkeletalMesh)
	{
		return;
	}

	ACharacter* OwnerCharacter = GetOwnerCharacter();

	if (OwnerCharacter == nullptr)
	{
		return;
	}

	bInitializedSkeletalMesh = true;

	// 피부 메테리얼 적용
	UMaterialInterface* LoadedMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.Material'/Game/Assets/Humans/Female/Materials/Skin/M_MT_Skin.M_MT_Skin'"));
	if (LoadedMat)
	{
		SkinMatRef = UMaterialInstanceDynamic::Create(LoadedMat, this);
		OwnerCharacter->GetMesh()->SetMaterialByName(TEXT("skin"), SkinMatRef);
	}

	FashionMeshs.SetNum((int32)EFashionType::Count);

	for (int32 i = 0; i < FashionMeshs.Num(); i++)
	{
		FashionMeshs[i] = NewObject<USkeletalMeshComponent>(OwnerCharacter, *(FString::Printf(TEXT("SKM_")) + StaticEnum<EFashionType>()->GetValueAsString((EFashionType)i)));
		FashionMeshs[i]->SetupAttachment(OwnerCharacter->GetMesh(), TEXT("RootSocket")); // 부모 설정, 로컬 좌표 (0,0,0) snaptotarget
		FashionMeshs[i]->SetCollisionProfileName(TEXT("NoCollision")); // 충돌 프로필 설정
		FashionMeshs[i]->bWantsInitializeComponent = true;
		FashionMeshs[i]->bBlendPhysics = true;
		FashionMeshs[i]->RegisterComponent();
		//FashionMeshs[i]->AddTickPrerequisiteComponent(OwnerCharacter->GetMesh());
	}

	ArmorMeshs.SetNum((int32)EArmorType::Count);

	for (int32 i = 0; i < ArmorMeshs.Num(); i++)
	{
		ArmorMeshs[i] = NewObject<UStaticMeshComponent>(OwnerCharacter, *(FString::Printf(TEXT("SKM_")) + StaticEnum<EArmorType>()->GetValueAsString((EArmorType)i)));
		ArmorMeshs[i]->SetupAttachment(OwnerCharacter->GetMesh(), TEXT("RootSocket")); // 부모 설정, 로컬 좌표 (0,0,0) snaptotarget
		ArmorMeshs[i]->SetCollisionProfileName(TEXT("NoCollision")); 
		ArmorMeshs[i]->bWantsInitializeComponent = true;
		ArmorMeshs[i]->RegisterComponent();
		//ArmorMeshs[i]->AddTickPrerequisiteComponent(OwnerCharacter->GetMesh());
	}

	UtilityMeshs.SetNum((int32)EUtilityType::Count);

	for (int32 i = 0; i < UtilityMeshs.Num(); i++)
	{
		UtilityMeshs[i] = NewObject<UStaticMeshComponent>(OwnerCharacter, *(FString::Printf(TEXT("SKM_")) + StaticEnum<EUtilityType>()->GetValueAsString((EUtilityType)i)));
		UtilityMeshs[i]->SetupAttachment(OwnerCharacter->GetMesh(), TEXT("RootSocket")); // 부모 설정, 로컬 좌표 (0,0,0) snaptotarget
		UtilityMeshs[i]->SetCollisionProfileName(TEXT("NoCollision"));
		UtilityMeshs[i]->bWantsInitializeComponent = true;
		UtilityMeshs[i]->RegisterComponent();
		//UtilityMeshs[i]->AddTickPrerequisiteComponent(OwnerCharacter->GetMesh());
	}

	//FAttachmentTransformRules TransformRules2 = FAttachmentTransformRules(
	//	EAttachmentRule::KeepRelative,
	//	EAttachmentRule::KeepRelative,
	//	EAttachmentRule::KeepRelative,
	//	true
	//);

	// 패션 매쉬 위치 선정
	for (int32 i = 0; i < FashionMeshs.Num(); i++)
	{
		if ((EFashionType)i == EFashionType::Hair)
		{
			FAttachmentTransformRules TransformRules = FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepWorld,
				true
			);

			FashionMeshs[i]->AttachToComponent(OwnerCharacter->GetMesh(), TransformRules, TEXT("Socket_Hair")); // 로컬 위치 -> 월드 좌표로 변환 후 헤드에 부착
		}
		else
		{
			// 부모 매쉬의 애니메이션 포지션을 따라간다.
			// 4.27 이전 구버전 : SetLeaderPoseComponent
			// 4.27 이후 신버전 : SetMasterPoseComponent 
			FashionMeshs[i]->SetMasterPoseComponent(OwnerCharacter->GetMesh()); // 부모 매쉬 위치를 상대적으로 따라감
		}
	}

	// 기본 복장 메쉬 설정
	if (FashionMeshs[(int32)EFashionType::Whole]->GetSkeletalMeshAsset() == nullptr)
	{
		UPUBGCharacterDefaultFashion* DefaultFashion = UPUBGAssetManager::GetAssetByName<UPUBGCharacterDefaultFashion>(TEXT("DefaultFashionData"));

		USkeletalMesh* LoadedDefaultFashionMesh = nullptr;
		UTexture* LoadedDefaultFashionMaskTexture = nullptr;

		if (FashionMeshs[(int32)EFashionType::ClothTop]->GetSkeletalMeshAsset() == nullptr)
		{
			DefaultFashion->GetFashionMesh(EFashionType::ClothTop, LoadedDefaultFashionMesh, LoadedDefaultFashionMaskTexture);
			FashionMeshs[(int32)EFashionType::ClothTop]->SetSkeletalMesh(LoadedDefaultFashionMesh);
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Top"), LoadedDefaultFashionMaskTexture);
		}
		if (FashionMeshs[(int32)EFashionType::ClothBottom]->GetSkeletalMeshAsset() == nullptr)
		{
			DefaultFashion->GetFashionMesh(EFashionType::ClothBottom, LoadedDefaultFashionMesh, LoadedDefaultFashionMaskTexture);
			FashionMeshs[(int32)EFashionType::ClothBottom]->SetSkeletalMesh(LoadedDefaultFashionMesh);
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Bottom"), LoadedDefaultFashionMaskTexture);
		}
	}

	// 기본 헤어 설정
	if (FashionMeshs[(int32)EFashionType::Hair]->GetSkeletalMeshAsset() == nullptr)
	{
		UPUBGCharacterDefaultFashion* DefaultFashion = UPUBGAssetManager::GetAssetByName<UPUBGCharacterDefaultFashion>(TEXT("DefaultFashionData"));

		USkeletalMesh* LoadedDefaultFashionMesh = nullptr;
		UTexture* LoadedDefaultFashionMaskTexture = nullptr;

		DefaultFashion->GetFashionMesh(EFashionType::Hair, LoadedDefaultFashionMesh, LoadedDefaultFashionMaskTexture);
		FashionMeshs[(int32)EFashionType::Hair]->SetSkeletalMesh(LoadedDefaultFashionMesh);
	}
}

void UPUBGCosmeticManagerComponent::ReplaceSkeletalMesh(EFashionType FashionType, USkeletalMesh* InSkeletalMesh, UTexture* InMaskTexture)
{
	UPUBGCharacterDefaultFashion* DefaultFashion = UPUBGAssetManager::GetAssetByName<UPUBGCharacterDefaultFashion>(TEXT("DefaultFashionData"));
	UTexture* DefaultFullMaskTexture = DefaultFashion->GetDefaultMaskTexture();
	uint8 IsEquip = (InSkeletalMesh != nullptr);

	// 매쉬 적용
	FashionMeshs[(int32)FashionType]->SetSkeletalMesh(InSkeletalMesh);

	// 장착/비장착
	switch (FashionType)
	{
	case EFashionType::Cap:
		break;
	case EFashionType::ClothTop:
		// 장착일시 상의 마스크 처리
		if (IsEquip)
		{
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Top"), (InMaskTexture ? InMaskTexture : DefaultFullMaskTexture));
		}
		break;
	case EFashionType::ClothBottom:
		// 장착일시 하의 마스크 처리
		if (IsEquip)
		{
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Bottom"), (InMaskTexture ? InMaskTexture : DefaultFullMaskTexture));
		}
		break;
	case EFashionType::Whole:
		// 장착일시 한벌옷 마스크 처리
		if (IsEquip)
		{
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Top"), (InMaskTexture ? InMaskTexture : DefaultFullMaskTexture));
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Bottom"), DefaultFullMaskTexture);

			// 강제 하의 매쉬 제거
			FashionMeshs[(int32)EFashionType::ClothTop]->SetSkeletalMesh(nullptr);
			FashionMeshs[(int32)EFashionType::ClothBottom]->SetSkeletalMesh(nullptr);
		}
		break;
	case EFashionType::Shoes:
		SkinMatRef->SetTextureParameterValue(TEXT("Mask Foot"), (InMaskTexture ? InMaskTexture : DefaultFullMaskTexture));
		break;
	}

	// 기본 복장으로 변경
	// 한벌옷 없음
	if (FashionMeshs[(int32)EFashionType::Whole]->GetSkeletalMeshAsset() == nullptr)
	{
		USkeletalMesh* LoadedDefaultFashionMesh = nullptr;
		UTexture* LoadedDefaultFashionMaskTexture = nullptr;

		// 상의 없으면 기본 속옷으로
		if (FashionMeshs[(int32)EFashionType::ClothTop]->GetSkeletalMeshAsset() == nullptr)
		{
			DefaultFashion->GetFashionMesh(EFashionType::ClothTop, LoadedDefaultFashionMesh, LoadedDefaultFashionMaskTexture);
			FashionMeshs[(int32)EFashionType::ClothTop]->SetSkeletalMesh(LoadedDefaultFashionMesh);
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Top"), LoadedDefaultFashionMaskTexture);
		}
		// 하의 없으면 기본 속옷으로
		if (FashionMeshs[(int32)EFashionType::ClothBottom]->GetSkeletalMeshAsset() == nullptr)
		{
			DefaultFashion->GetFashionMesh(EFashionType::ClothBottom, LoadedDefaultFashionMesh, LoadedDefaultFashionMaskTexture);
			FashionMeshs[(int32)EFashionType::ClothBottom]->SetSkeletalMesh(LoadedDefaultFashionMesh);
			SkinMatRef->SetTextureParameterValue(TEXT("Mask Bottom"), LoadedDefaultFashionMaskTexture);
		}
	}
}

void UPUBGCosmeticManagerComponent::ReplaceArmorMesh(EArmorType ArmorType, UStaticMesh* InArmorMesh, FName SocketName)
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();

	if (OwnerCharacter == nullptr)
	{
		return;
	}

	switch (ArmorType)
	{
	case EArmorType::Helmet:
		ArmorMeshs[(int32)EArmorType::Helmet]->SetStaticMesh(InArmorMesh);
		FashionMeshs[(int32)EFashionType::Hair]->SetVisibility(!InArmorMesh);
		break;
	case EArmorType::Vest:
		ArmorMeshs[(int32)EArmorType::Vest]->SetStaticMesh(InArmorMesh);
		break;
	}

	if (InArmorMesh && SocketName.IsNone() == false)
	{
		FTransform SocketTransform = OwnerCharacter->GetMesh()->GetSocketTransform(SocketName);
		ArmorMeshs[(int32)ArmorType]->SetWorldTransform(SocketTransform);

		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepRelative,
			true
		);
		ArmorMeshs[(int32)ArmorType]->AttachToComponent(OwnerCharacter->GetMesh(), TransformRules, SocketName);
	}
}

void UPUBGCosmeticManagerComponent::ReplaceUtilityMesh(EUtilityType UtilityType, UStaticMesh* InUtilityMesh, FName SocketName)
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();

	if (OwnerCharacter == nullptr)
	{
		return;
	}

	switch (UtilityType)
	{
	case EUtilityType::Backpack:
		UtilityMeshs[(int32)EUtilityType::Backpack]->SetStaticMesh(InUtilityMesh);
		break;
	case EUtilityType::Belt:
		UtilityMeshs[(int32)EUtilityType::Belt]->SetStaticMesh(InUtilityMesh);
		break;
	}

	if (InUtilityMesh && SocketName.IsNone() == false)
	{
		// 소켓 위치로 이동
		FTransform SocketTransform = OwnerCharacter->GetMesh()->GetSocketTransform(SocketName);
		UtilityMeshs[(int32)UtilityType]->SetWorldTransform(SocketTransform);

		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepRelative,
			true
		);

		// 해당 소켓에 부모로 설정
		UtilityMeshs[(int32)UtilityType]->AttachToComponent(OwnerCharacter->GetMesh(), TransformRules, SocketName);
	}
}

ACharacter* UPUBGCosmeticManagerComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

