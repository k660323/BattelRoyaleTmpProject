// Kakanu Portfolio Project


#include "Characters/PUBGCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Attributes/PUBGCombatSet.h"
#include "PUBGGameplayTags.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MoveStopped, "Gameplay.MoveStopped");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_RotationStopped, "Gameplay.RotationStopped");

// 콘솔모드에서 치트 모드일때 지면 감지 거리 설정 가능
namespace PUBGCharacter
{
	static float GroundTraceDistance = 100000.0f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("PUBGCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
}

UPUBGCharacterMovementComponent::UPUBGCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPUBGCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		const FVector OriginalAcceleration = Acceleration;
		Super::SimulateMovement(DeltaTime);
		Acceleration = OriginalAcceleration;
	}
	else
	{
		Super::SimulateMovement(DeltaTime);
	}
}

bool UPUBGCharacterMovementComponent::CanAttemptJump() const
{
	// Same as UCharacterMovementComponent's implementation but without the crouch check
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());// Falling included for double-jump and non-zero jump hold time, but validated by character.
}

const FPUBGCharacterGroundInfo& UPUBGCharacterMovementComponent::GetGroundInfo()
{
	// 소유자가 없거나 또는 동일 프레임일 경우
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - PUBGCharacter::GroundTraceDistance - CapsuleHalfHeight));
	
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PUBGCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(QueryParams, ResponseParam);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);
		
		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = PUBGCharacter::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}

void UPUBGCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	bHasReplicatedAcceleration = true;
	Acceleration = InAcceleration;
}

FRotator UPUBGCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped) || ASC->HasMatchingGameplayTag(TAG_Gameplay_RotationStopped))
		{
			return FRotator(0.0f, 0.0f, 0.0f);
		}
	}

	return Super::GetDeltaRotation(DeltaTime);
}

float UPUBGCharacterMovementComponent::GetMaxSpeed() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped) || ASC->HasMatchingGameplayTag(TAG_Gameplay_MoveStopped))
		{
			return 0.0f;
		}

		const UAttributeSet* AttributeSet = ASC->GetAttributeSet(UPUBGCombatSet::StaticClass());
		if (const UPUBGCombatSet* CombatSet = Cast<UPUBGCombatSet>(AttributeSet))
		{
			float MoveSpeed = CombatSet->GetMoveSpeed();
			float MoveSpeedPercent = CombatSet->GetMoveSpeedPercent();

			float MaxMoveSpeed = FMath::Max(0.f, MoveSpeed);
			MaxMoveSpeed = FMath::Max(0.f, MaxMoveSpeed + MaxMoveSpeed * (MoveSpeedPercent * 0.01f));
		
			switch (MovementMode)
			{
			case MOVE_Walking:
			case MOVE_NavWalking:
			{
				float DirectionCheck;
				float DirectionDot = GetOwner()->GetActorForwardVector().Dot(Velocity.GetSafeNormal());

				if (DirectionDot < 0.25f)
				{
					if (DirectionDot > -0.25f)
					{
						DirectionCheck = MaxMoveSpeed * LeftRightMovePercent;
					}
					else
					{
						DirectionCheck = MaxMoveSpeed * BackwardMovePercent;
					}
				}
				else
				{
					DirectionCheck = MaxMoveSpeed;
				}

				bool IsProne = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Prone);
				bool IsAim = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Aim);
				bool IsDrink = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Drink);
				bool IsSprint = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Sprint);
				bool IsWalk = ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Walk);

				float CrouchCheck = IsCrouching() ? DirectionCheck * CrouchMovePercent : DirectionCheck;
				float ProneCheck = IsProne ? CrouchCheck * ProneMovePercent : CrouchCheck;
				float AimCheck = (IsAim) ? ProneCheck * AimMovePercent : ProneCheck;
				float DrinkCheck = IsDrink ? AimCheck * DrinkMovePercent : AimCheck;
				float SprintCheck = IsSprint ? DrinkCheck * SprintMovePercent : DrinkCheck;
				float WalkCheck = IsWalk ? SprintCheck * WalkMovePercent : SprintCheck;

				GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("MaxSpeed: %f"), WalkCheck));

				return WalkCheck;
			}
			case MOVE_Falling:
				return MaxMoveSpeed;
			case MOVE_Swimming:
				return MaxSwimSpeed;
			case MOVE_Flying:
				return MaxFlySpeed;
			case MOVE_Custom:
				return MaxCustomMovementSpeed;
			case MOVE_None:
			default:
				return 0.f;
			}
		}
	}

	return Super::GetMaxSpeed();
}
