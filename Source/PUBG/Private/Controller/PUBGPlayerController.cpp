// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PUBGPlayerController.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGInputData.h"
#include "EnhancedInputSubSystems.h"
#include "Input/PUBGEnhancedInputComponent.h"
#include "PUBGGameplayTags.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "Player/PUBGPlayerState.h"
#include "Curves/CurveFloat.h"
#include "Utils/PUBGFunctionLibrary.h"
#include "Components/TimelineComponent.h"
#include "Input/PUBGEnhancedPlayerInput.h"
#include "Item/Managers/PUBGItemManagerComponent.h"
#include "Camera/CameraSpringArmTimelineComponent.h"
#include "PUBG.h"

// TMP
#include "Item/Managers/PUBGInventoryManagerComponent.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Item/Managers/PUBGWeaponAccManagerComponent.h"
#include "Item/Fragments/PUBGItemFragment_GunAcc.h"
#include "Item/PUBGItemInstance.h"
#include "Actors/PUBGEquipmentBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerController)

APUBGPlayerController::APUBGPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ItemManagerComponent(nullptr)
	, IsFreeViewMove(false)
	, CachedPressedRotator(FRotator::ZeroRotator)
	, CachedReleaseRotator(FRotator::ZeroRotator)
	, BottomValue(290.f)
	, TopValue(75.f)
{
	ItemManagerComponent = CreateDefaultSubobject<UPUBGItemManagerComponent>(TEXT("ItemManagerComponent"));
	CameraSpringArmTimelineComponent = CreateDefaultSubobject<UCameraSpringArmTimelineComponent>(TEXT("CameraSpringArmTimelineComponent"));
}

// ��ǲ Ŭ���� ����
void APUBGPlayerController::InitInputSystem()
{
	OverridePlayerInputClass = UPUBGEnhancedPlayerInput::StaticClass();
	Super::InitInputSystem();
}

void APUBGPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void APUBGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// ������ ��� Ű ���ε�
	if (const UPUBGInputData* InputData = UPUBGAssetManager::Get().GetAssetByName<UPUBGInputData>("InputData"))
	{
		// �ϵ���� �Է��� Subsystem�� �޾���
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		check(Subsystem);
		Subsystem->AddMappingContext(InputData->NativeInputMappingContextData.InputMappingContext, InputData->NativeInputMappingContextData.Priority);

		// SubSystem�� ���� Ű�� UEnhancedInputComponent �޾� �Լ��� �����Ѵ�.
		auto* EnhancedInputComponent = CastChecked<UPUBGEnhancedInputComponent>(InputComponent);

		// �⺻Ű ����
		EnhancedInputComponent->BindNativeInputAction(InputData, PUBGGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
		EnhancedInputComponent->BindNativeInputAction(InputData, PUBGGameplayTags::Input_Turn, ETriggerEvent::Triggered, this, &ThisClass::Input_Turn);
		EnhancedInputComponent->BindNativeInputAction(InputData, PUBGGameplayTags::Input_FreeView, ETriggerEvent::Started, this, &ThisClass::Input_FreeViewStarted);
		EnhancedInputComponent->BindNativeInputAction(InputData, PUBGGameplayTags::Input_FreeView, ETriggerEvent::Completed, this, &ThisClass::Input_FreeViewReleased);

		// Ư�� Ű ����
		EnhancedInputComponent->BindAbilityInputAction(InputData, this, &ThisClass::Input_AbilityInputStarted , &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
	}

	PUBG_LOG(LogPUBGNetwork, Log, TEXT("Call SetupInputComponent"));
}


void APUBGPlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	if (GetCharacter() == nullptr)
	{
		return;
	}

	// �Է� x,y [(x(w,s), y(a,d)]
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	FRotator MovementRotation = (IsFreeViewMove) ? 
		FRotator(0.0f, CachedPressedRotator.Yaw, 0.0f): FRotator(0.0f, GetControlRotation().Yaw, 0.0f);

	if (MovementVector.X != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		GetCharacter()->AddMovementInput(ForwardDirection, MovementVector.X);
	}

	if (MovementVector.Y != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		GetCharacter()->AddMovementInput(RightDirection, MovementVector.Y);
	}

}

void APUBGPlayerController::Input_Turn(const FInputActionValue& InputActionValue)
{
	if (GetCharacter() == nullptr)
	{
		return;
	}

	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	// �¿�
	if (LookAxisVector.X != 0.0f)
	{
		GetCharacter()->AddControllerYawInput(LookAxisVector.X);
	}

	// �� �Ʒ�
	if (LookAxisVector.Y != 0.0f)
	{
		if (LimitPitchAngle(-LookAxisVector.Y))
		{
			GetCharacter()->AddControllerPitchInput(-LookAxisVector.Y);
		}
	}

}

void APUBGPlayerController::Input_FreeViewStarted(const FInputActionValue& InputActionValue)
{
	// TODO : ASC �±׷� ó��
	if (IsFreeViewMove)
	{
		return;
	}

	if (GetCharacter() == nullptr)
	{
		return;
	}

	IsFreeViewMove = true;
	CachedPressedRotator = GetControlRotation();

	if (APUBGPlayerCharacter* PlayerCharacter = Cast<APUBGPlayerCharacter>(GetCharacter()))
	{
		PlayerCharacter->SetCameraFreeViewMode(true);
	}
}

void APUBGPlayerController::Input_FreeViewReleased(const FInputActionValue& InputActionValue)
{
	if (IsFreeViewMove == false)
	{
		return;
	}

	if (GetCharacter() == nullptr)
	{
		return;
	}

	CachedReleaseRotator = GetControlRotation();
	CameraSpringArmTimelineComponent->EndDelegate.AddDynamic(this, &ThisClass::FreeViewTimelineEnd);
	CameraSpringArmTimelineComponent->PlayTimeline(FreeViewMode, 0);
}

void APUBGPlayerController::Input_AbilityInputStarted(FGameplayTag InInputTag)
{
	if (GetCharacter() == nullptr)
	{
		return;
	}

	if (APUBGPlayerCharacter* PlayerCharacter = Cast<APUBGPlayerCharacter>(GetCharacter()))
	{
		if (UPUBGAbilitySystemComponent* ASC = PlayerCharacter->GetPUBGAbilitySystemComponent())
		{
			ASC->OnAbilityTagStarted(InInputTag);
		}
	}
}

void APUBGPlayerController::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	if (GetCharacter() == nullptr)
	{
		return;
	}

	if (APUBGPlayerCharacter* PlayerCharacter = Cast<APUBGPlayerCharacter>(GetCharacter()))
	{
		if (UPUBGAbilitySystemComponent* ASC = PlayerCharacter->GetPUBGAbilitySystemComponent())
		{
			ASC->OnAbilityTagPressed(InInputTag); 
		}
	}
}

void APUBGPlayerController::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	if (GetCharacter() == nullptr)
	{
		return;
	}

	if (APUBGPlayerCharacter* PlayerCharacter = Cast<APUBGPlayerCharacter>(GetCharacter()))
	{
		if (UPUBGAbilitySystemComponent* ASC = PlayerCharacter->GetPUBGAbilitySystemComponent())
		{
			ASC->OnAbilityTagReleased(InInputTag);
		}
	}
}

void APUBGPlayerController::FreeViewTimelineEnd()
{
	IsFreeViewMove = false;

	if (APUBGPlayerCharacter* PlayerCharacter = Cast<APUBGPlayerCharacter>(GetCharacter()))
	{
		PlayerCharacter->SetCameraFreeViewMode(false);
	}
}

void APUBGPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UPUBGAbilitySystemComponent* PUBGASC = GetPUBGAbilitySystemComponent())
	{
		PUBGASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UPUBGAbilitySystemComponent* APUBGPlayerController::GetPUBGAbilitySystemComponent() const
{
	const APUBGPlayerState* PUBGPS = GetPUBGPlayerState();
	return (PUBGPS ? PUBGPS->GetPUBGAbilitySystemComponent() : nullptr);
}

APUBGPlayerState* APUBGPlayerController::GetPUBGPlayerState() const
{
	return CastChecked<APUBGPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

bool APUBGPlayerController::LimitPitchAngle(const float Axis)
{
	if (UPUBGFunctionLibrary::NativeDoesActorHaveTag(GetCharacter(), PUBGGameplayTags::Status_Prone))
	{
		BottomValue = 350.0f;
		TopValue = 15.0f;
	}
	else if (UPUBGFunctionLibrary::NativeDoesActorHaveTag(GetCharacter(), PUBGGameplayTags::Status_Crouch))
	{
		BottomValue = 290.0f;
		TopValue = 50.0f;
	}
	else
	{
		BottomValue = 290.0f;
		TopValue = 75.0f;
	}

	float pitch = GetControlRotation().Pitch;
	
	// ���� �ʰ�
	if ((pitch > BottomValue || pitch < TopValue) == false)
	{
		// ���� pitch�� �����̿� �ִ� ���� ������ ���� ����
		if ((FMath::Abs(pitch - TopValue) < FMath::Abs(pitch - BottomValue)))
		{
			SetControlRotation(FRotator(TopValue, GetControlRotation().Yaw, GetControlRotation().Roll));
		}
		else
		{
			SetControlRotation(FRotator(BottomValue, GetControlRotation().Yaw, GetControlRotation().Roll));
		}
	}

	pitch = GetControlRotation().Pitch - Axis;

	return (pitch > BottomValue || pitch < TopValue);
}

FRotator APUBGPlayerController::GetPressedRotator()
{
	return CachedPressedRotator;
}

FRotator APUBGPlayerController::GetReleaseRotator()
{
	return CachedReleaseRotator;
}

void APUBGPlayerController::TestEquipAccInventoryFirstItem()
{
	// �κ��丮 ��������
	UPUBGInventoryManagerComponent* InventoryManager = GetCharacter()->FindComponentByClass<UPUBGInventoryManagerComponent>();

	// ù��° ������ �ν��Ͻ� �����ϰ� �����´�.
	UPUBGItemInstance* RemovedItemInstance = InventoryManager->TryRemoveItem(0, 1);
	if (RemovedItemInstance)
	{
		// Main ���⿡ ����
		UPUBGEquipmentManagerComponent* EquipmentManager = GetCharacter()->FindComponentByClass<UPUBGEquipmentManagerComponent>();
		APUBGEquipmentBase* EquipmentActor = EquipmentManager->GetEntryEquipmentActor(EEquipmentSlotType::Main);
		UPUBGWeaponAccManagerComponent* WeaponAccManager = EquipmentActor->FindComponentByClass<UPUBGWeaponAccManagerComponent>();
	
		const UPUBGItemFragment_GunAcc* GunAccFragment = RemovedItemInstance->FindFragmentByClass<UPUBGItemFragment_GunAcc>();

		WeaponAccManager->EquipAcc(GunAccFragment->GunAccType, RemovedItemInstance);
	}
}
