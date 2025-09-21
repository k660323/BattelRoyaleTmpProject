#include "Interaction/Abilities/Task/PUBGAbilityTask_CheckForInvalidIntraction.h"
#include "Characters/PUBGBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilityTask_CheckForInvalidIntraction)


UPUBGAbilityTask_CheckForInvalidIntraction::UPUBGAbilityTask_CheckForInvalidIntraction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPUBGAbilityTask_CheckForInvalidIntraction* UPUBGAbilityTask_CheckForInvalidIntraction::CheckForInvalidIntraction(UGameplayAbility* OwningAbility, float AcceptanceAngle, float AcceptanceDistance)
{
	UPUBGAbilityTask_CheckForInvalidIntraction* Task = NewAbilityTask<UPUBGAbilityTask_CheckForInvalidIntraction>(OwningAbility);
	Task->AcceptanceAngle = AcceptanceAngle;
	Task->AcceptanceDistance = AcceptanceDistance;

	return Task;
}

void UPUBGAbilityTask_CheckForInvalidIntraction::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	CachedDirection2D = GetAvatarActor()->GetActorForwardVector().GetSafeNormal2D();
	CachedActorLocation = GetAvatarActor()->GetActorLocation();

	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ThisClass::CheckForInvalidIntraction, 0.5f, true);
}

void UPUBGAbilityTask_CheckForInvalidIntraction::OnDestroy(bool bInOwnerFinished)
{
	GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UPUBGAbilityTask_CheckForInvalidIntraction::CheckForInvalidIntraction()
{
	APUBGBaseCharacter* BaseCharacter = Cast<APUBGBaseCharacter>(Ability->GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* MovementComponent = BaseCharacter->GetCharacterMovement();

	if (BaseCharacter && MovementComponent)
	{
		bool IsValidAngle2D = (CalculateAngle2D() <= AcceptanceAngle);
		bool IsValidDistanceXY = FVector::DistSquared2D(CachedActorLocation, BaseCharacter->GetActorLocation()) <= (AcceptanceDistance * AcceptanceDistance);
		bool IsValidDistanceZ = FMath::Abs(CachedActorLocation.Z - BaseCharacter->GetActorLocation().Z) <= (AcceptanceDistance + FMath::Abs(MovementComponent->GetCrouchedHalfHeight() - BaseCharacter->BaseUnscaledCapsuleHalfHeight));
	
		if (IsValidAngle2D && IsValidDistanceXY && IsValidDistanceZ)
		{
			return;
		}
	}

	OnInvalidIntraction.Broadcast();

	EndTask();
}

float UPUBGAbilityTask_CheckForInvalidIntraction::CalculateAngle2D()
{
	AActor* Actor = GetAvatarActor();

	if (Actor)
	{
		FVector CurrentDirection2D = Actor->GetActorForwardVector().GetSafeNormal2D();
		float Length = CachedDirection2D.Dot(CurrentDirection2D);
		float RadianDegree = FMath::Acos(Length);
		float Degree = FMath::RadiansToDegrees(RadianDegree);

		return Degree;
	}

	return 0.0f;
}
