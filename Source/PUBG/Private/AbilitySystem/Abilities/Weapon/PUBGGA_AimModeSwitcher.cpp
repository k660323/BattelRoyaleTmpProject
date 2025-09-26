// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Weapon/PUBGGA_AimModeSwitcher.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitForTick.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "PUBGGameplayTags.h"

UPUBGGA_AimModeSwitcher::UPUBGGA_AimModeSwitcher(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bSwitch(false)
	, CurWaitTime(0.0f)
	, ThresHold(0.2f)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnHold;
}

void UPUBGGA_AimModeSwitcher::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurWaitTime = 0.f;

	if (bSwitch == false)
	{
		WaitForTick = UPUBGAbilityTask_WaitForTick::WaitForTick(this);
		WaitForTick->OnTick.AddDynamic(this, &ThisClass::OnTick);
		WaitForTick->ReadyForActivation();
	}

	if (UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
		WaitInputRelease->ReadyForActivation();
	}
}

void UPUBGGA_AimModeSwitcher::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UPUBGGA_AimModeSwitcher::OnTick(float DeltaTime)
{
	CurWaitTime += DeltaTime;

	if (CurWaitTime >= ThresHold)
	{
		UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo();

		if (ASC == nullptr)
		{
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			return;
		}

		WaitForTick->EndTask();
		WaitForTick = nullptr;
		// 견착 모드
		bSwitch = ASC->TryActivateAbilityByClass(ADSAbilityClass);
	}
}


void UPUBGGA_AimModeSwitcher::OnInputRelease(float TimeHeld)
{
	UPUBGAbilitySystemComponent* ASC = GetPUBGAbilitySystemComponentFromActorInfo();

	if (ASC == nullptr)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Aim_Scope))
	{
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(ScopeAbilityClass);
		if (Spec && Spec->IsActive())
		{
			ASC->CancelAbility(Spec->Ability);
			bSwitch = false;
		}
	}
	else if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Aim_ADS))
	{
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(ADSAbilityClass);
		if (Spec && Spec->IsActive())
		{
			ASC->CancelAbility(Spec->Ability);
			bSwitch = false;
		}
	}
	else
	{
		// 스코프 모드
		bSwitch = ASC->TryActivateAbilityByClass(ScopeAbilityClass);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

