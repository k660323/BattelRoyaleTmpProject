// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_Death.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "Characters/PUBGBaseCharacter.h"
#include "PUBGGameplayTags.h"

UPUBGGA_Death::UPUBGGA_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_Death);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPUBGGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	check(ActorInfo);

	APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo();
	if (BaseCharacter == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	BaseCharacter->SetIsDead(true);

	UPUBGAbilitySystemComponent* PUBGASC = CastChecked<UPUBGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (PUBGASC == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(PUBGGameplayTags::Ability_Behavior_SurvivesDeath);

	PUBGASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);
	SetCanBeCanceled(false);

	if (ChangeActivationGroup(EPUBGAbilityActivationGroup::Exclusive_Blocking) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UPUBGA_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	
}

void UPUBGGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo())
	{
		BaseCharacter->SetIsDead(false);
	}
}
