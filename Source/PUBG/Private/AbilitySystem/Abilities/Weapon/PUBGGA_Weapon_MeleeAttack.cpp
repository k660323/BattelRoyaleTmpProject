// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Weapon/PUBGGA_Weapon_MeleeAttack.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/PUBGEquipmentBase.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Weapon_MeleeAttack)

UPUBGGA_Weapon_MeleeAttack::UPUBGGA_Weapon_MeleeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;
	ActivationGroup = EPUBGAbilityActivationGroup::Exclusive_Replaceable;
}

void UPUBGGA_Weapon_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, PUBGGameplayTags::GameplayEvent_Trace, nullptr, false, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnTargetDataReady);
		GameplayEventTask->ReadyForActivation();
	}

	if (UAbilityTask_WaitGameplayEvent* GameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, PUBGGameplayTags::GameplayEvent_Montage_End, nullptr, true, true))
	{
		GameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnMontageEventTriggered);
		GameplayEventTask->ReadyForActivation();
	}

	if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UpperBody"), AttackMontage, GetCachedAttackRate(), NAME_None, false, 1.0f, 0.f, false))
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageFinished);
		PlayMontageTask->ReadyForActivation();
	}
}

void UPUBGGA_Weapon_MeleeAttack::HandleMontageEvent(FGameplayEventData Payload)
{
	OnMontageFinished();
}

void UPUBGGA_Weapon_MeleeAttack::OnTargetDataReady(FGameplayEventData Payload)
{
	APUBGEquipmentBase* WeaponActor = const_cast<APUBGEquipmentBase*>(Cast<APUBGEquipmentBase>(Payload.Instigator));
	if (WeaponActor == nullptr)
	{
		return;
	}

	UPUBGAbilitySystemComponent* SourceASC = GetPUBGAbilitySystemComponentFromActorInfo();
	if (SourceASC == nullptr)
	{
		return;
	}

	// 현재 ASC에 존재하는 어빌리티인지 체크
	if (SourceASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(Payload.TargetData)));
		
		// 피격된 액터 인덱스 배열
		TArray<int32> HitIndexes;
		ParseTargetData(LocalTargetDataHandle, HitIndexes); // 생략해도 될듯?

		float Damage = GetEquipmentStatValue(PUBGGameplayTags::SetByCaller_BaseDamage, WeaponActor);
		
		for (int32 HitIndex : HitIndexes)
		{
			FHitResult HitResult = *LocalTargetDataHandle.Data[HitIndex]->GetHitResult();
			ProcessHitResult(HitResult, Damage, WeaponActor);
		}
	}
}

void UPUBGGA_Weapon_MeleeAttack::OnMontageEventTriggered(FGameplayEventData Payload)
{
	HandleMontageEvent(Payload);
}

void UPUBGGA_Weapon_MeleeAttack::OnMontageFinished()
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
