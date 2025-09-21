// Kakanu Portfolio Project


#include "PUBGTypes/PUBGStructTypes.h"
#include "GameplayEffect.h"
#include "AttributeSet.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGStructTypes)

bool FPUBGAbilitySet::IsValid() const
{
	return AbilityToGrant != nullptr;
}

bool FPUBGGameplayTagAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}

bool FPUBGGameplayEffectSet::IsValid() const
{
	return GameplayEffect != nullptr;
}

bool FPUBGAttributeSetSet::IsValid() const
{
	return AttributeSet != nullptr;
}
