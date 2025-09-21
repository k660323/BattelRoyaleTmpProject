// Kakanu Portfolio Project

#pragma once

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/Attributes/PUBGAttributeSet.h"
#include "PUBGVitalSet.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_PUBG_Damage_Message);
/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGVitalSet : public UPUBGAttributeSet
{
	GENERATED_BODY()
	
public:
	UPUBGVitalSet();

	// 레플리케이션 변수 등록
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Effect 적용전 (자폭 여부에 따른 데미지 처리)
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	// Effect 적용후
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// 변경전 값 보정
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	// 변경전 값 보정
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// 최대값 수정시 현재값을 최대값으로 변경
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	// 속성값 보정
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
	// Init, Get, Set, Find 매크로
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Stamina);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, EnergyBoost);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxEnergyBoost);

protected:
	// 콜백 함수
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_EnergyBoost(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxEnergyBoost(const FGameplayAttributeData& OldValue);

private:
	// 속성 값
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyBoost, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData EnergyBoost;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergyBoost, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxEnergyBoost;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingLimitedHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingEnergyBoost);
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingDamage);

private:
	// 속성 값 (수치 변경용)
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData IncomingHealth;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData IncomingLimitedHealth;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData IncomingStamina;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData IncomingEnergyBoost;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData IncomingDamage;

public:
	// mutable : const함수에서도 델리게이트 브로드캐스팅 가능
	mutable FPUBGAttributeEvent OnHealthChanged;
	mutable FPUBGAttributeEvent OnMaxHealthChanged;
	mutable FPUBGAttributeEvent OnOutOfHealth;
	mutable FPUBGAttributeEvent OnEnergyBoostChanged;
	mutable FPUBGAttributeEvent OnMaxEnergyBoostChanged;
private:
	// 체력이 0이하로 내려가면 더이상 브로드 캐스팅하지 않는 변수
	bool bOutOfHealth;

	// 수치 바뀌기전 속성값들
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	float LimitedHealth = 70.0f;
};
