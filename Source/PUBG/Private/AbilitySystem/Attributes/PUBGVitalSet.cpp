// Kakanu Portfolio Project


#include "AbilitySystem/Attributes/PUBGVitalSet.h"
#include "PUBGGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Messages/PUBGVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGVitalSet)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_PUBG_Damage_Message, "PUBG.Damage.Message");

UPUBGVitalSet::UPUBGVitalSet()
{
	bOutOfHealth = false;
	MaxHealthBeforeAttributeChange = 0.0f;
	HealthBeforeAttributeChange = 0.0f;
}

void UPUBGVitalSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Stamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxStamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EnergyBoost, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxEnergyBoost, COND_OwnerOnly, REPNOTIFY_Always);
}

bool UPUBGVitalSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (Super::PreGameplayEffectExecute(Data) == false)
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// 자폭 여부
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
			
			// 자폭일경우 수치를 0으로 하고 false 반환한다.
			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && bIsDamageFromSelfDestruct == false)
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

// 비 배포 코드
#if !UE_BUILD_SHIPPING
			if (Data.Target.HasMatchingGameplayTag(PUBGGameplayTags::Cheat_GodMode) && bIsDamageFromSelfDestruct == false)
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif  // #if !UE_BUILD_SHIPPING
		}
	}

	// 수치 적용전 값 캐싱
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UPUBGVitalSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
	float MinimumHealth = 0.0f;

// 비 배포 코드
#if !UE_BUILD_SHIPPING
	if (bIsDamageFromSelfDestruct == false &&
		(Data.Target.HasMatchingGameplayTag(PUBGGameplayTags::Cheat_GodMode) || Data.Target.HasMatchingGameplayTag(PUBGGameplayTags::Cheat_UnlimitedHealth)))
	{
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	// Effect를 실행한 가해자
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	// Effect 발생 시킨 객체
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// Effect 처리는 서버에서만
			// 콜백 델리게이트 느낌 (이펙트, UI 반응 처리 등) 
			FPUBGVerbMessage Message;
			Message.Verb = TAG_PUBG_Damage_Message;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		SetHealth(FMath::Clamp(GetHealth() - GetIncomingDamage(), MinimumHealth, GetMaxHealth()));
		SetIncomingDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetIncomingHealth(), MinimumHealth, GetMaxHealth()));
		SetIncomingHealth(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingLimitedHealthAttribute())
	{
		if (GetHealth() < LimitedHealth)
		{
			float value = FMath::Min(GetHealth() + GetIncomingLimitedHealth(), LimitedHealth);
			SetHealth(value);
		}

		SetIncomingLimitedHealth(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina() + GetIncomingStamina(), 0.0f, GetMaxStamina()));
		SetIncomingStamina(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingEnergyBoostAttribute())
	{
		SetEnergyBoost(FMath::Clamp(GetEnergyBoost() + GetIncomingEnergyBoost(), 0.0f, GetMaxEnergyBoost()));
		SetIncomingEnergyBoost(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetEnergyBoostAttribute())
	{
		SetEnergyBoost(FMath::Clamp(GetEnergyBoost(), 0.0f, GetMaxEnergyBoost()));
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}

	// 최대 체력 변화
	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// 체력 변화
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// 체력 0이하로 갈시
	if ((GetHealth() <= 0.0f) && bOutOfHealth == false)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// 체력 0이하시 브로드 캐스트 호출 여부를 제어하는 변수
	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UPUBGVitalSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPUBGVitalSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPUBGVitalSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// 최대체력 변경시 체력 변경
	if (Attribute == GetMaxHealthAttribute())
	{
		UPUBGAbilitySystemComponent* PUBGASC = GetPUBGAbilitySystemComponent();
		check(PUBGASC);

		PUBGASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
	}
	// 최대 스테미나 변경시 스테미나 변경
	else if (Attribute == GetMaxStaminaAttribute())
	{
		if (GetStamina() > NewValue)
		{
			UPUBGAbilitySystemComponent* PUBGASC = GetPUBGAbilitySystemComponent();
			check(PUBGASC);

			PUBGASC->ApplyModToAttribute(GetStaminaAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfHealth && GetHealth() > 0.0f)
	{
		bOutOfHealth = false;
	}
}

void UPUBGVitalSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 체력
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	// 최대 체력
	else if (Attribute == GetMaxHealthAttribute())
	{
		// 최대최력은 최소 1.0f부터
		NewValue = FMath::Max(1.0f, NewValue);
	}
	// 스태미너
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	// 최대 스테미너
	else if (Attribute == GetMaxStaminaAttribute())
	{
		// 최대 스테미너는 최소 1.0f 부터
		NewValue = FMath::Max(1.0f, NewValue);
	}
	else if (Attribute == GetEnergyBoostAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxEnergyBoost());
	}
	else if (Attribute == GetMaxEnergyBoostAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
}

void UPUBGVitalSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	// 전달받은 값을 ASC에 알림
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);

	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	// 바인딩한 함수들 호출
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	
	if (bOutOfHealth == false && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UPUBGVitalSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);

	// (현재값 - 이전값), 이전값, 현재값
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());
}

void UPUBGVitalSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Stamina, OldValue);
}

void UPUBGVitalSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxStamina, OldValue);
}

void UPUBGVitalSet::OnRep_EnergyBoost(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EnergyBoost, OldValue);

	const float CurrentEnergyBoost = GetEnergyBoost();
	const float EstimatedMagnitude = CurrentEnergyBoost - OldValue.GetCurrentValue();

	// 바인딩한 함수들 호출
	OnEnergyBoostChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentEnergyBoost);
}

void UPUBGVitalSet::OnRep_MaxEnergyBoost(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxEnergyBoost, OldValue);

	OnMaxEnergyBoostChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxEnergyBoost() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxEnergyBoost());
}
