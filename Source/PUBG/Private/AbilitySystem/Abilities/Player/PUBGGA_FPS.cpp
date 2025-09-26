// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGGA_FPS.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "PUBGGameplayTags.h"

UPUBGGA_FPS::UPUBGGA_FPS(const FObjectInitializer& ObjectInitalizer)
	: Super(ObjectInitalizer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;
	
	AbilityTags.AddTag(PUBGGameplayTags::Ability_FPS);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_FPS);
}

bool UPUBGGA_FPS::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UPUBGGA_FPS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APUBGPlayerCharacter* Character = Cast<APUBGPlayerCharacter>(GetBaseCharacterFromActorInfo());
	if (Character == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	Character->SetFPSMode(true);

	if (UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this))
	{
		WaitInputPress->OnPress.AddDynamic(this, &ThisClass::OnPressed);
		WaitInputPress->ReadyForActivation();
	}
}

void UPUBGGA_FPS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (APUBGPlayerCharacter* Character = Cast<APUBGPlayerCharacter>(GetBaseCharacterFromActorInfo()))
	{
		Character->SetFPSMode(false);
	}
}

void UPUBGGA_FPS::OnPressed(float TimeWaited)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
