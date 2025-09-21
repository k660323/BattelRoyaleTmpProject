// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Weapon/PUBGGA_Weapon_MeleeCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitInputStart.h"
#include "AbilitySystemComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Weapon_MeleeCombo)

UPUBGGA_Weapon_MeleeCombo::UPUBGGA_Weapon_MeleeCombo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationGroup = EPUBGAbilityActivationGroup::Exclusive_Replaceable;
}

void UPUBGGA_Weapon_MeleeCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bAllowInputPress = false;
	bInputStart = false;


	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, PUBGGameplayTags::GameplayEvent_Input_Pressed, nullptr, false, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnAllowInputPress);
		GameplayEventTask->ReadyForActivation();
	}
	
	InputStart();
}

void UPUBGGA_Weapon_MeleeCombo::HandleMontageEvent(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo) == false)
		return;

	
	bool bCanContinue = NextAbilityClass && bAllowInputPress && bInputStart;
	if (bCanContinue)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->TryActivateAbilityByClass(NextAbilityClass);
		}
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UPUBGGA_Weapon_MeleeCombo::InputStart()
{
	if (UPUBGAbilityTask_WaitInputStart* InputStartTask = UPUBGAbilityTask_WaitInputStart::WaitInputStart(this))
	{
		InputStartTask->OnStart.AddDynamic(this, &ThisClass::OnInputStart);
		InputStartTask->ReadyForActivation();
	}
}

void UPUBGGA_Weapon_MeleeCombo::OnAllowInputPress(FGameplayEventData Payload)
{
	bAllowInputPress = true;
}

void UPUBGGA_Weapon_MeleeCombo::OnInputStart()
{
	if (bAllowInputPress)
	{
		bInputStart = true;
	}
	else
	{
		InputStart();
	}
}
