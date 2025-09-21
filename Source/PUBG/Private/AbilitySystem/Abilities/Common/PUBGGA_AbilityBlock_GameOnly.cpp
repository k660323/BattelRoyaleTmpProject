// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_AbilityBlock_GameOnly.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_AbilityBlock_GameOnly)

UPUBGGA_AbilityBlock_GameOnly::UPUBGGA_AbilityBlock_GameOnly(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(PUBGGameplayTags::Ability_Block_AbilityGameOnly);
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::Type::GameplayEvent;
	TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_Block_AbilityGameOnly;
	AbilityTriggers.Add(TriggerData);
}

void UPUBGGA_AbilityBlock_GameOnly::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}, TriggerEventData->EventMagnitude, false);
}

void UPUBGGA_AbilityBlock_GameOnly::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}
