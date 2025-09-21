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

	// ���ø����̼� ���� ���
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Effect ������ (���� ���ο� ���� ������ ó��)
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	// Effect ������
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// ������ �� ����
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	// ������ �� ����
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// �ִ밪 ������ ���簪�� �ִ밪���� ����
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	// �Ӽ��� ����
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
	// Init, Get, Set, Find ��ũ��
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Stamina);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, EnergyBoost);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxEnergyBoost);

protected:
	// �ݹ� �Լ�
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
	// �Ӽ� ��
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
	// �Ӽ� �� (��ġ �����)
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
	// mutable : const�Լ������� ��������Ʈ ��ε�ĳ���� ����
	mutable FPUBGAttributeEvent OnHealthChanged;
	mutable FPUBGAttributeEvent OnMaxHealthChanged;
	mutable FPUBGAttributeEvent OnOutOfHealth;
	mutable FPUBGAttributeEvent OnEnergyBoostChanged;
	mutable FPUBGAttributeEvent OnMaxEnergyBoostChanged;
private:
	// ü���� 0���Ϸ� �������� ���̻� ��ε� ĳ�������� �ʴ� ����
	bool bOutOfHealth;

	// ��ġ �ٲ���� �Ӽ�����
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	float LimitedHealth = 70.0f;
};
