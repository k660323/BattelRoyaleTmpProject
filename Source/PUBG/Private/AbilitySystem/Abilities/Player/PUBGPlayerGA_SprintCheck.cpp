// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGPlayerGA_SprintCheck.h"
#include "Characters/PUBGBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerGA_SprintCheck)

UPUBGPlayerGA_SprintCheck::UPUBGPlayerGA_SprintCheck(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_Sprint_Check);
}

void UPUBGPlayerGA_SprintCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsLocallyControlled() == false)
	{
		return;
	}

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


	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (AbilitySystem == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	EPUBGDirection Direction;
	FVector MovementVector;
	GetMovementDirection(Direction, MovementVector);

	/*
	bool bSuccessfullyFoundAttribute = false;
	float RemainValue = UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(AbilitySystem, SprintCostAttribute, bSuccessfullyFoundAttribute);

	if (CharacterMovement->IsFalling() || Direction != EPUBGDirection::Forward || bSuccessfullyFoundAttribute == false || RemainValue <= 10.f)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	*/

	if (CharacterMovement->IsFalling() || Direction != EPUBGDirection::Forward)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		Server_RequestSprint(Direction, MovementVector);
		SendSprintActiveEvent(Direction, MovementVector);
	}
}

void UPUBGPlayerGA_SprintCheck::Server_RequestSprint_Implementation(EPUBGDirection Direction, FVector MovementVector)
{
	SendSprintActiveEvent(Direction, MovementVector);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPUBGPlayerGA_SprintCheck::SendSprintActiveEvent(EPUBGDirection Direction, FVector MovementVector)
{
	FHitResult HitResult;
	HitResult.Item = (int32)Direction;
	HitResult.Normal = MovementVector;

	FGameplayEventData Payload;
	Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(HitResult);

	SendGameplayEvent(PUBGGameplayTags::Ability_Sprint_Active, Payload);
}
