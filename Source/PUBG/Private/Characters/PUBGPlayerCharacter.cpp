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

	// ĳ���� �ݶ��̴�(�Ž�) ����
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// ī�޶� ���� ------------------------
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 200.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 25.f, 100.f));
	// �ε巴�� ȸ��
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.f;

	// ��Ʈ�ѷ� ȸ������ �ݿ��Ѵ�.
	SpringArmComponent->bUsePawnControlRotation = true;

	TPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCameraComponent"));

	TPSCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	// ��Ʈ�ѷ��� ȸ���� �ݿ����� �ʴ´�.
	TPSCameraComponent->bUsePawnControlRotation = false;
	// ------------------------------------

	// 
	// ��Ʈ�ѷ� ȸ������ ���� ȸ��
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	// �Է� ���� �������� ȸ��x
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

	// �ʱ�ȭ (������ ����, ���־� ����)
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
		// ��Ʈ�ѷ� ȸ������ ���� ȸ��
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		// ��Ʈ�ѷ� ȸ������ ���� ȸ��
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		//GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	Server_SetCameraFreeViewMode(IsOn);
}

void APUBGPlayerCharacter::Server_SetCameraFreeViewMode_Implementation(bool IsOn)
{
	if (IsOn)
	{
		// ��Ʈ�ѷ� ȸ������ ���� ȸ��
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		//GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else
	{
		// ��Ʈ�ѷ� ȸ������ ���� ȸ��
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

// �������� ����(Owner ����)�� Ŭ���̾�Ʈ���� ȣ��
void APUBGPlayerCharacter::OnRep_Owner()
{
	Super::OnRep_Owner();

}

void APUBGPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// PS �Ҵ�� �ʱ�ȭ
	InitializeAbilitySystem();
	Server_InitializeDefaultAbility();
}

