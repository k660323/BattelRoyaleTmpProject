// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_StateTransformEnd.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_StateTransformEnd)

UPUBGGA_StateTransformEnd::UPUBGGA_StateTransformEnd(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CancelAbilitiesWithTag.AddTag(PUBGGameplayTags::Ability_StateTransform);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::Type::GameplayEvent;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_StateTransform_End;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPUBGGA_StateTransformEnd::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
