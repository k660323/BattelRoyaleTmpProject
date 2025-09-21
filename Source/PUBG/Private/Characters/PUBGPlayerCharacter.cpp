// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PUBGPlayerCharacter.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystem/PUBGAbilityTagRelationshipMapping.h"
#include "Player/PUBGPlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "System/PUBGAssetManager.h"
#include "PUBGGameplayTags.h"

APUBGPlayerCharacter::APUBGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = HasAuthority();

	// 캐릭터 콜라이더(매쉬) 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// 카메라 설정 ------------------------
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 200.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 25.f, 100.f));
	// 부드럽게 회전
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.f;

	// 컨트롤러 회전값을 반영한다.
	SpringArmComponent->bUsePawnControlRotation = true;

	TPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCameraComponent"));

	TPSCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	// 컨트롤러의 회전을 반영하지 않는다.
	TPSCameraComponent->bUsePawnControlRotation = false;
	// ------------------------------------

	// 
	// 컨트롤러 회전값에 따라 회전
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	// 입력 방향 기준으로 회전x
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

}

void APUBGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void APUBGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (Controller == nullptr)
		{
			return;
		}

		AimOffsetControllerYaw = Controller->GetControlRotation().Yaw;
	}
}

void APUBGPlayerCharacter::InitializeAbilitySystem()
{
	APUBGPlayerState* PUBGPlayerState = GetPlayerState<APUBGPlayerState>();
	check(PUBGPlayerState);

	PUBGAbilitySystemComponent = PUBGPlayerState->GetPUBGAbilitySystemComponent();
	check(PUBGAbilitySystemComponent);

	// 초기화 (데이터 액터, 비주얼 액터)
	PUBGAbilitySystemComponent->InitAbilityActorInfo(PUBGPlayerState, this);

	if (UPUBGAbilityTagRelationshipMapping* TagRelationshipMappingData = UPUBGAssetManager::Get().GetAssetByName<UPUBGAbilityTagRelationshipMapping>("TagRelcationsShips_Player"))
	{
		PUBGAbilitySystemComponent->SetTagRelationshipMapping(TagRelationshipMappingData);
	}

	OnAbilitySystemInitialized();
}

void APUBGPlayerCharacter::UninitializeAbilitySystem()
{
	if (!PUBGAbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (PUBGAbilitySystemComponent->GetAvatarActor() == this)
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(PUBGGameplayTags::Ability_Behavior_SurvivesDeath);

		PUBGAbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		PUBGAbilitySystemComponent->ClearAbilityInput();
		PUBGAbilitySystemComponent->RemoveAllGameplayCues();

		if (PUBGAbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			PUBGAbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			PUBGAbilitySystemComponent->ClearActorInfo();
		}

		PUBGAbilitySystemComponent->SetTagRelationshipMapping(nullptr);

		// OnAbilitySystemUninitialized.Broadcast();

		Server_UnInitializeDefaultAbility();
	}

	OnAbilitySystemUninitialized();

	PUBGAbilitySystemComponent = nullptr;
}

void APUBGPlayerCharacter::Server_InitializeDefaultAbility_Implementation()
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (const UDataAsset_StartUpDataBase* StartUpData = UPUBGAssetManager::Get().GetAssetByName<UDataAsset_StartUpDataBase>("PlayerStartUpData"))
	{
		AbilityGrantedHandles.ClearByAbilitySystem(PUBGAbilitySystemComponent);
		StartUpData->GiveToAbilitySystemComponent(PUBGAbilitySystemComponent, &AbilityGrantedHandles, this);
	}
}

void APUBGPlayerCharacter::Server_UnInitializeDefaultAbility_Implementation()
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (!PUBGAbilitySystemComponent)
	{
		return;
	}

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(PUBGGameplayTags::Ability_Behavior_SurvivesDeath);

	PUBGAbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
	PUBGAbilitySystemComponent->ClearAbilityInput();
	PUBGAbilitySystemComponent->RemoveAllGameplayCues();

	AbilityGrantedHandles.ClearByAbilitySystem(PUBGAbilitySystemComponent);
}

void APUBGPlayerCharacter::SetCameraFreeViewMode(bool IsOn)
{
	if (IsOn)
	{
		// 컨트롤러 회전값에 따라 회전
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		// 컨트롤러 회전값에 따라 회전
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	Server_SetCameraFreeViewMode(IsOn);
}

void APUBGPlayerCharacter::Server_SetCameraFreeViewMode_Implementation(bool IsOn)
{
	if (IsOn)
	{
		// 컨트롤러 회전값에 따라 회전
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		// 컨트롤러 회전값에 따라 회전
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void APUBGPlayerCharacter::SetProneSpringArm(bool IsOn)
{
	if (IsOn)
	{
		SpringArmComponent->SetRelativeLocation(FVector(0.f, 25.f, 0.f));
	}
	else
	{
		SpringArmComponent->SetRelativeLocation(FVector(0.f, 25.f, 100.f));
	}
}

// 서버에서 빙의(Owner 변경)시 클라이언트에서 호출
void APUBGPlayerCharacter::OnRep_Owner()
{
	Super::OnRep_Owner();

}

void APUBGPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// PS 할당시 초기화
	InitializeAbilitySystem();
	Server_InitializeDefaultAbility();
}

