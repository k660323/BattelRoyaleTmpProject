// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGPlayerGA_Jump.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PUBGDefine.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerGA_Jump)

UPUBGPlayerGA_Jump::UPUBGPlayerGA_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 점프 어빌리티 복제
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;
}

void UPUBGPlayerGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 로컬만 실행
	if (IsLocallyControlled() == false)
		return;

	APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (PlayerCharacter == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}

	UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();
	if (CharacterMovement == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	EPUBGDirection Direction;
	FVector MovementVector;
	GetMovementDirection(Direction, MovementVector);

	if ((Direction != EPUBGDirection::None && Direction != EPUBGDirection::Forward) || CharacterMovement->IsFalling())
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo) == false)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// 서버가 아닐시
	if(HasAuthority(&CurrentActivationInfo) == false)
	{
		Server_RequestJump();
		StartJump();
		//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UPUBGPlayerGA_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		PlayerCharacter->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLandedCallback);
	}
}

void UPUBGPlayerGA_Jump::Server_RequestJump_Implementation()
{
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	StartJump();
	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPUBGPlayerGA_Jump::StartJump()
{
	if (APUBGPlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		PlayerCharacter->LandedDelegate.AddDynamic(this, &ThisClass::OnLandedCallback);
		if (PlayerCharacter->IsLocallyControlled() && PlayerCharacter->bPressedJump == false)
		{
			PlayerCharacter->UnCrouch();
			PlayerCharacter->Jump();
		}
	}
}

void UPUBGPlayerGA_Jump::OnLandedCallback(const FHitResult& Hit)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
