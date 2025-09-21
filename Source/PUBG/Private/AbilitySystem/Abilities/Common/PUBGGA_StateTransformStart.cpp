// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_StateTransformStart.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_StateTransformStart)

UPUBGGA_StateTransformStart::UPUBGGA_StateTransformStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(PUBGGameplayTags::Ability_StateTransform);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_StateTransform);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::Type::GameplayEvent;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_StateTransform_Start;
		AbilityTriggers.Add(TriggerData);
	}

}