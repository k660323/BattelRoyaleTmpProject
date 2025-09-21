// Kakanu Portfolio Project


#include "AbilitySystem/Attributes/PUBGCombatSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGCombatSet)

UPUBGCombatSet::UPUBGCombatSet()
{
}

void UPUBGCombatSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseDamage, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseHealth, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseLimitedHealth, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseStamina, COND_OwnerOnly, REPNOTIFY_OnChanged);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeedPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeedPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HeadResistancePercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BodyResistancePercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Resourcefulness, COND_OwnerOnly, REPNOTIFY_OnChanged);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraClipSize, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ReplaceClipTimePercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RecoilRecoveryPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, VerticalRecoilPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HorizontalRecoilPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AimingtimePercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HipfireAccuracyPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AimedShootingAccuracyPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, FirstShotRecoilPercent, COND_OwnerOnly, REPNOTIFY_OnChanged);

}

void UPUBGCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPUBGCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPUBGCombatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	
}

void UPUBGCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseDamage, OldValue);
}

void UPUBGCombatSet::OnRep_BaseHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseHealth, OldValue);
}

void UPUBGCombatSet::OnRep_BaseLimitedHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseLimitedHealth, OldValue);
}

void UPUBGCombatSet::OnRep_BaseStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseStamina, OldValue);
}

void UPUBGCombatSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);

}

void UPUBGCombatSet::OnRep_MoveSpeedPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeedPercent, OldValue);
}

void UPUBGCombatSet::OnRep_AttackSpeedPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeedPercent, OldValue);
}

void UPUBGCombatSet::OnRep_HeadResistancePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HeadResistancePercent, OldValue);
}

void UPUBGCombatSet::OnRep_BodyResistancePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BodyResistancePercent, OldValue);
}

void UPUBGCombatSet::OnRep_Resourcefulness(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Resourcefulness, OldValue);
}

void UPUBGCombatSet::OnRep_ExtraClipSize(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraClipSize, OldValue);
}

void UPUBGCombatSet::OnRep_ReplaceClipTimePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ReplaceClipTimePercent, OldValue);
}

void UPUBGCombatSet::OnRep_RecoilRecoveryPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, RecoilRecoveryPercent, OldValue);
}

void UPUBGCombatSet::OnRep_VerticalRecoilPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, VerticalRecoilPercent, OldValue);
}

void UPUBGCombatSet::OnRep_HorizontalRecoilPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HorizontalRecoilPercent, OldValue);
}

void UPUBGCombatSet::OnRep_AimingtimePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AimingtimePercent, OldValue);
}

void UPUBGCombatSet::OnRep_HipfireAccuracyPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HipfireAccuracyPercent, OldValue);
}

void UPUBGCombatSet::OnRep_AimedShootingAccuracyPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AimedShootingAccuracyPercent, OldValue);
}

void UPUBGCombatSet::OnRep_FirstShotRecoilPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, FirstShotRecoilPercent, OldValue);
}
