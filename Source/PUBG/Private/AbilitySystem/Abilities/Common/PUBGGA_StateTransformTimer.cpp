// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_StateTransformTimer.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_StateTransformTimer)

UPUBGGA_StateTransformTimer::UPUBGGA_StateTransformTimer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(PUBGGameplayTags::Ability_StateTransform);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_StateTransform);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::Type::GameplayEvent;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_StateTransform_Timer;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPUBGGA_StateTransformTimer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}, TriggerEventData->EventMagnitude, false);
}

void UPUBGGA_StateTransformTimer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
