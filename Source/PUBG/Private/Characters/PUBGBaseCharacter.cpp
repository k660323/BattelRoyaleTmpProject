// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PUBGBaseCharacter.h"
#include "Characters/PUBGCharacterMovementComponent.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Utils/PUBGFunctionLibrary.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGCharacterDefaultFashion.h"
#include "UObject/EnumProperty.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Item/Managers/PUBGCosmeticManagerComponent.h"
#include "Item/Managers/PUBGInventoryManagerComponent.h"
#include "GameFrameWork/Controller.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/PUBGVerbMessageHelpers.h"
#include "Messages/PUBGVerbMessage.h"
#include "GameplayEffect.h"
#include "AbilitySystem/Attributes/PUBGVitalSet.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGBaseCharacter)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PUBG_Elimination_Message, "PUBG.Elimination.Message");


// Sets default values
APUBGBaseCharacter::APUBGBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPUBGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	NetCullDistanceSquared = FMath::Square(100000);
	bOnlyRelevantToOwner = false;

	PUBGCharacterMovement = Cast<UPUBGCharacterMovementComponent>(GetCharacterMovement());

	PUBGActiveEquipmentManagerComponent = CreateDefaultSubobject<UPUBGActiveEquipmentManagerComponent>(TEXT("ActiveEquipmentManagerComponent"));
	PUBGEquipmentManagerComponent = CreateDefaultSubobject<UPUBGEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));
	PUBGInventoryManagerComponent = CreateDefaultSubobject<UPUBGInventoryManagerComponent>(TEXT("InventoryManagerComponent"));
	
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionProfileName("PUBGPawnMesh");
	MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionProfileName("PUBGPawnCapsule");
}

void APUBGBaseCharacter::InitializeAbilitySystem()
{

}

void APUBGBaseCharacter::UninitializeAbilitySystem()
{

}

void APUBGBaseCharacter::OnAbilitySystemInitialized()
{
	UPUBGAbilitySystemComponent* PUBGASC = GetPUBGAbilitySystemComponent();
	check(PUBGASC);

	HealthSet = PUBGASC->GetSet<UPUBGVitalSet>();
	if (!HealthSet)
	{
		UE_LOG(LogTemp, Error, TEXT("PUBGHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(this));
		return;
	}

	// Register to listen for attribute changes.
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	//PUBGASC->SetNumericAttributeBase(UPUBGVitalSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	//PUBGASC->SetNumericAttributeBase(UPUBGVitalSet::GetManaAttribute(), HealthSet->GetMaxMana());
	//PUBGASC->SetNumericAttributeBase(UPUBGVitalSet::GetStaminaAttribute(), HealthSet->GetMaxStamina());

	// 강제 호출
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void APUBGBaseCharacter::OnAbilitySystemUninitialized()
{
	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
}

void APUBGBaseCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

#if WITH_CLIENT_CODE || WITH_EDITOR

	if (!GetWorld() || (GetWorld() && (GetWorld()->WorldType == EWorldType::EditorPreview)))
	{
		return;
	}

	UPUBGCosmeticManagerComponent* CosmeticManagerComponent = NewObject<UPUBGCosmeticManagerComponent>(this, TEXT("CosmeticManagerComponent"));
	CosmeticManagerComponent->bWantsInitializeComponent = true;
	CosmeticManagerComponent->RegisterComponent();

#endif // WITH_CLIENT_CODE || WITH_EDITOR
}

void APUBGBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APUBGBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	BaseUnscaledCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
}

// 서버 빙의 및 서버 초기화 (Owner 설정 -> 클라 OnRep 호출)
void APUBGBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 빙의 시점 초기화
	InitializeAbilitySystem();
}

void APUBGBaseCharacter::UnPossessed()
{
	Super::UnPossessed();

	UninitializeAbilitySystem();
}

void APUBGBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	Super::EndPlay(EndPlayReason);
}

void APUBGBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AimOffsetControllerYaw, COND_SimulatedOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bIsDead, COND_None, REPNOTIFY_Always);
	// DOREPLIFETIME_CONDITION(ThisClass, bIsProned, COND_SimulatedOnly);
}

void APUBGBaseCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;

		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void APUBGBaseCharacter::SetIsDead(bool IsDead)
{
	if (HasAuthority() == false)
	{
		return;
	}

	bIsDead = IsDead;
	OnRep_Dead();
}

void APUBGBaseCharacter::OnRep_Dead()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	USkeletalMeshComponent* SKM = GetMesh();
	UCharacterMovementComponent* MoveComponent = GetCharacterMovement();

	if (Capsule == nullptr || SKM == nullptr || MoveComponent == nullptr)
	{
		return;
	}

	if (bIsDead)
	{
		MoveComponent->SetJumpAllowed(false);
		Capsule->SetCollisionProfileName(TEXT("NoCollision"));
		Capsule->SetEnableGravity(false);

		SKM->SetCollisionProfileName(TEXT("Ragdoll"));
		// SKM->SetIsReplicated(true); 클라만 존재하는 객체를 레플리케이션하면 멈춤
		SKM->SetAllBodiesSimulatePhysics(true);
		SKM->WakeAllRigidBodies();

		SKM->bBlendPhysics = true; // 물리 본 위치를 소켓에 반영

		if (Controller)
		{
			if (Controller->IsA(AAIController::StaticClass()))
			{
				MoveComponent->bUseControllerDesiredRotation = false;
			}
			else
			{
				Controller->SetIgnoreMoveInput(true);
				Controller->SetIgnoreLookInput(true);
			}
		}
	}
	else
	{
		MoveComponent->SetJumpAllowed(true);

		Capsule->SetCollisionProfileName(TEXT("PUBGPawnCapsule"));
		Capsule->SetEnableGravity(true);
		SKM->SetCollisionProfileName(TEXT("PUBGPawnMesh"));
		// SKM->SetIsReplicated(false); 클라만 존재하는 객체를 레플리케이션하면 멈춤
		SKM->SetAllBodiesSimulatePhysics(false);
		SKM->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SKM->bBlendPhysics = false;

		SKM->AttachToComponent(Capsule, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SKM->SetRelativeLocation(FVector::ZeroVector);
		SKM->SetRelativeRotation(FRotator::ZeroRotator);

		if (Controller)
		{
			if (Controller->IsA(AAIController::StaticClass()))
			{
				MoveComponent->bUseControllerDesiredRotation = true;
			}
			else
			{
				Controller->SetIgnoreMoveInput(false);
				Controller->SetIgnoreLookInput(false);
			}
		}
	}
}

UAbilitySystemComponent* APUBGBaseCharacter::GetAbilitySystemComponent() const
{
	return GetPUBGAbilitySystemComponent();
}

void APUBGBaseCharacter::OnRep_ReplicatedAcceleration()
{
	if (UPUBGCharacterMovementComponent* PUBGMovementComponent = Cast<UPUBGCharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel = PUBGMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYRadians) * MaxAccel / 255.0f; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0f; // [0,255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0f;	// [-127 ~ 127] -> [-MaxAccel, MaxAccel]

		PUBGMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void APUBGBaseCharacter::RecalculateBaseEyeHeight()
{
	if (!IsCrouched())
	{
		Super::RecalculateBaseEyeHeight();
	}
	else
	{
		BaseEyeHeight = CrouchedEyeHeight;
	}
}

void APUBGBaseCharacter::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void APUBGBaseCharacter::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void APUBGBaseCharacter::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (PUBGAbilitySystemComponent && DamageEffectSpec)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;
			Payload.EventTag = PUBGGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = PUBGAbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(PUBGAbilitySystemComponent, true);
			PUBGAbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// Send a standardized verb message that other systems can observe
		{
			FPUBGVerbMessage Message;
			Message.Verb = TAG_PUBG_Elimination_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Message.Target = Cast<UObject>(UPUBGVerbMessageHelpers::GetPlayerStateFromObject(PUBGAbilitySystemComponent->GetAvatarActor()));
			Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
	}

#endif // #if WITH_SERVER_CODE
}
