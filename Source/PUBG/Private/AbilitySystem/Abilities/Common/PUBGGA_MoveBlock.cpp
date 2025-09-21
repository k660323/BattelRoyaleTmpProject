// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_MoveBlock.h"
#include "Characters/PUBGBaseCharacter.h"
#include "GameFramework/Controller.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_MoveBlock)

UPUBGGA_MoveBlock::UPUBGGA_MoveBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPUBGGA_MoveBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// (클라, 서버) 둘다 실행
	APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo();
	if (BaseCharacter == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	AController* Controller = BaseCharacter->GetController();
	if (Controller == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	if (Controller->IsA(AAIController::StaticClass()))
	{
		if (UCharacterMovementComponent* CharacterMovement = BaseCharacter->GetCharacterMovement())
		{
			CharacterMovement->bUseControllerDesiredRotation = false;
		}
	}
	else
	{
		Controller->SetIgnoreMoveInput(true);
		//BaseCharacter->bUseControllerRotationYaw = false;
	}


	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}, TriggerEventData->EventMagnitude, false);
}

void UPUBGGA_MoveBlock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo())
	{
		if (AController* Controller = BaseCharacter->GetController())
		{
			if (Controller->IsA(AAIController::StaticClass()))
			{
				if (UCharacterMovementComponent* CharacterMovement = BaseCharacter->GetCharacterMovement())
				{
					CharacterMovement->bUseControllerDesiredRotation = true;
				}
			}
			else
			{
				Controller->SetIgnoreMoveInput(false);
				//BaseCharacter->bUseControllerRotationYaw = true;
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
