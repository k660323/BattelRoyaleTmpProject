// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGPlayerGA_Sprint.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Utils/PUBGFunctionLibrary.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitForTick.h"
#include "Kismet/KismetMathLibrary.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerGA_Sprint)

UPUBGPlayerGA_Sprint::UPUBGPlayerGA_Sprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// bServerRespectsRemoteAbilityCancellation = false;
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;
	AbilityTags.AddTag(PUBGGameplayTags::Ability_Sprint_Active);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Sprint);

	// CDO일때면 설정
	//if (HasAnyFlags(RF_ClassDefaultObject))
	//{
	//	FAbilityTriggerData TriggerData;
	//	TriggerData.TriggerTag = PUBGGameplayTags::Ability_Sprint_Active;
	//	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	//	AbilityTriggers.Add(TriggerData);
	//}
}

void UPUBGPlayerGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APUBGBaseCharacter* PUBGCharacter = GetBaseCharacterFromActorInfo();
	if (PUBGCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	UCharacterMovementComponent* CharacterMovement = PUBGCharacter->GetCharacterMovement();
	if (CharacterMovement == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CharacterMovement->IsFalling())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (IsLocallyControlled())
	{
		EPUBGDirection Direction;
		FVector MovementVector;
		GetMovementDirection(Direction, MovementVector);

		if (Direction != EPUBGDirection::Forward)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}
	}
	else if(HasAuthority(&CurrentActivationInfo))
	{
		FVector Direction = UKismetMathLibrary::Normal(CharacterMovement->Velocity);
		FTransform Transform = PUBGCharacter->GetActorTransform();
		// 월드 -> 로컬
		FVector LocalDirection = UKismetMathLibrary::InverseTransformDirection(Transform, Direction);

		if (LocalDirection.X <= ForwardThreshold)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}

	if (UPUBGAbilityTask_WaitForTick* TickTask = UPUBGAbilityTask_WaitForTick::WaitForTick(this))
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnTick);
		TickTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnReInputRelease);
		WaitInputRelease->ReadyForActivation();
	}

	// GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnSprintCommitTick, CommitInterval, true);
}

void UPUBGPlayerGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//if (RecoveryBlockEffectClass)
	//{
	//	const UGameplayEffect* GameplayEffectCDO = RecoveryBlockEffectClass->GetDefaultObject<UGameplayEffect>();
	//	ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, GameplayEffectCDO, 1);
	//}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPUBGPlayerGA_Sprint::OnTick(float DeltaTime)
{
	// 서버
	if (HasAuthority(&CurrentActivationInfo))
	{
		APUBGBaseCharacter* PUBGCharacter = GetBaseCharacterFromActorInfo();
		if (PUBGCharacter == nullptr)
		{
			return;
		}

		UCharacterMovementComponent* CharacterMovement = PUBGCharacter->GetCharacterMovement();
		if (CharacterMovement == nullptr)
		{
			return;
		}

		FVector Direction = UKismetMathLibrary::Normal(CharacterMovement->Velocity);
		FTransform Transform = PUBGCharacter->GetActorTransform();
		// 월드 -> 로컬
		FVector LocalDirection = UKismetMathLibrary::InverseTransformDirection(Transform, Direction);

		if (LocalDirection.X <= ForwardThreshold)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
	// 클라
	else
	{
		EPUBGDirection Direction;
		FVector MovementVector;
		GetMovementDirection(Direction, MovementVector);

		if (Direction != EPUBGDirection::Forward)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}

void UPUBGPlayerGA_Sprint::OnReInputRelease(float Time)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

//void UPUBGPlayerGA_Sprint::OnSprintCommitTick()
//{
//	if (K2_CommitAbilityCost() == false)
//	{
//		GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
//		K2_CommitAbilityCooldown(false, true);
//		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
//	}
//}
