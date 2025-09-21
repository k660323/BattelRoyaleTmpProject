// Kakanu Portfolio Project


#include "AbilitySystem/Executions/PUBGVitalExecution.h"
#include "AbilitySystem/Attributes/PUBGVitalSet.h"
#include "AbilitySystem/Attributes/PUBGCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGVitalExecution)

struct FVitalStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseLimitedHealthDef;
	FGameplayEffectAttributeCaptureDefinition BaseStaminaDef;

	FVitalStatics()
	{
		BaseHealthDef = FGameplayEffectAttributeCaptureDefinition(UPUBGCombatSet::GetBaseHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseLimitedHealthDef = FGameplayEffectAttributeCaptureDefinition(UPUBGCombatSet::GetBaseLimitedHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseStaminaDef = FGameplayEffectAttributeCaptureDefinition(UPUBGCombatSet::GetBaseStaminaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FVitalStatics& HealStatics()
{
	static FVitalStatics Statics;
	return Statics;
}

UPUBGVitalExecution::UPUBGVitalExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealthDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseLimitedHealthDef);
	RelevantAttributesToCapture.Add(HealStatics().BaseStaminaDef);
}

void UPUBGVitalExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// �±� ��������
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// �߰� ����
	float BaseHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealthDef, EvaluateParameters, BaseHealth);

	// �߰� ����(������ ����)
	float BaseLimitedHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseLimitedHealthDef, EvaluateParameters, BaseLimitedHealth);
	
	// �߰� ���׹̳�
	float BaseStamina = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseStaminaDef, EvaluateParameters, BaseStamina);

	// ȿ�� �߰� (VitalSet���� �� ����)
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPUBGVitalSet::GetIncomingHealthAttribute(), EGameplayModOp::Additive, BaseHealth));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPUBGVitalSet::GetIncomingLimitedHealthAttribute(), EGameplayModOp::Additive, BaseLimitedHealth));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPUBGVitalSet::GetIncomingStaminaAttribute(), EGameplayModOp::Additive, BaseStamina));

#endif // WITH_SERVER_CODE
}
