// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/PUBGPlayerGameplayAbility.h"
#include "Characters/PUBGPlayerCharacter.h"
#include "Controller/PUBGPlayerController.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerCharacter)

APUBGPlayerCharacter* UPUBGPlayerGameplayAbility::GetPlayerCharacterFromActorInfo()
{
	if (!CachedPUBGPlayerCharacter.IsValid())
	{
		CachedPUBGPlayerCharacter = Cast<APUBGPlayerCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedPUBGPlayerCharacter.IsValid() ? CachedPUBGPlayerCharacter.Get() : nullptr;
}

APUBGPlayerController* UPUBGPlayerGameplayAbility::GetPlayerControllerFromActorInfo()
{
	if (!CachedPUBGPlayerController.IsValid())
	{
		CachedPUBGPlayerController = Cast<APUBGPlayerController>(CurrentActorInfo->PlayerController);
	}

	return CachedPUBGPlayerController.IsValid() ? CachedPUBGPlayerController.Get() : nullptr;
}

FGameplayEffectSpecHandle UPUBGPlayerGameplayAbility::MakeHeroDamageEffectsSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount)
{
	check(EffectClass);

	FGameplayEffectContextHandle ContextHandle = GetPUBGAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetPUBGAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	/*EffectSpecHandle.Data->SetSetByCallerMagnitude(
		WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);*/

	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(
			InCurrentAttackTypeTag,
			InUsedComboCount
		);
	}

	return EffectSpecHandle;
}

bool UPUBGPlayerGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime)
{
	check(InCooldownTag.IsValid());

	FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());

	TArray<TPair<float, float>> TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);

	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
		TotalCooldownTime = TimeRemainingAndDuration[0].Value;
	}

	return RemainingCooldownTime > 0.f;
}
