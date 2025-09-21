// Kakanu Portfolio Project

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PUBGAttributeSet.h"
#include "PUBGCombatSet.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGCombatSet : public UPUBGAttributeSet
{
	GENERATED_BODY()
	
public:
	UPUBGCombatSet();

	// ���ø����̼� ���� ���
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

private:
	// �Ӽ��� ����
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, Dummy);

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Dummy;

public:
	
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseStamina);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseLimitedHealth);

protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseLimitedHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseStamina(const FGameplayAttributeData& OldValue);

private:
	// ����, �߰� ����, �߰� ���׹̳�//HideFromModifiers
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, meta = (AllowPrivateAccess="true"))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHealth, meta=(AllowPrivateAccess ="true"))
	FGameplayAttributeData BaseHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseLimitedHealth, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseLimitedHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseStamina, meta=(AllowPrivateAccess ="true"))
	FGameplayAttributeData BaseStamina;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeed);
	ATTRIBUTE_ACCESSORS(ThisClass, MoveSpeedPercent);
	ATTRIBUTE_ACCESSORS(ThisClass, AttackSpeedPercent);
	ATTRIBUTE_ACCESSORS(ThisClass, HeadResistancePercent);
	ATTRIBUTE_ACCESSORS(ThisClass, BodyResistancePercent);

protected:
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackSpeedPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HeadResistancePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BodyResistancePercent(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeedPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MoveSpeedPercent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeedPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData AttackSpeedPercent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HeadResistancePercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData HeadResistancePercent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BodyResistancePercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BodyResistancePercent;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, Resourcefulness);

protected:
	UFUNCTION()
	void OnRep_Resourcefulness(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resourcefulness, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Resourcefulness;


// ���� ���� ����
public:
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraClipSize); //
	ATTRIBUTE_ACCESSORS(ThisClass, ReplaceClipTimePercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, RecoilRecoveryPercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, VerticalRecoilPercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, HorizontalRecoilPercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, AimingtimePercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, HipfireAccuracyPercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, AimedShootingAccuracyPercent); //
	ATTRIBUTE_ACCESSORS(ThisClass, FirstShotRecoilPercent); //

protected:
	UFUNCTION()
	void OnRep_ExtraClipSize(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ReplaceClipTimePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_RecoilRecoveryPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_VerticalRecoilPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HorizontalRecoilPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AimingtimePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HipfireAccuracyPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AimedShootingAccuracyPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FirstShotRecoilPercent(const FGameplayAttributeData& OldValue);

private:
	// �߰� ��ź�� (���� �׼����� ����)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExtraClipSize, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData ExtraClipSize;

	// źâ ��ü �ð� �ۼ�Ʈ (���� �׼����� ����)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ReplaceClipTimePercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData ReplaceClipTimePercent;

	// �ݵ� ȸ�� �ۼ�Ʈ (���� �׼����� ����)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RecoilRecoveryPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData RecoilRecoveryPercent;

	// ���� �ݵ� (���� �׼����� ����)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_VerticalRecoilPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData VerticalRecoilPercent;

	// ���� �ݵ� (���� �׼����� ����)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HorizontalRecoilPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData HorizontalRecoilPercent;

	// ������ �ӵ� �ۼ�Ʈ
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AimingtimePercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData AimingtimePercent;

	// ������ ��� ��Ȯ��
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HipfireAccuracyPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData HipfireAccuracyPercent;

	// ���� ��� ��Ȯ��
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AimedShootingAccuracyPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData AimedShootingAccuracyPercent;

	// ��ź �ݵ� �ۼ�Ʈ
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FirstShotRecoilPercent, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData FirstShotRecoilPercent;
};
