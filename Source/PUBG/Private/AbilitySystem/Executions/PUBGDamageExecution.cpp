// Kakanu Portfolio Project


#include "AbilitySystem/Executions/PUBGDamageExecution.h"
#include "AbilitySystem/Attributes/PUBGCombatSet.h"
#include "AbilitySystem/Attributes/PUBGVitalSet.h"
#include "System/PUBGAssetManager.h"
#include "Data/PUBGDamageProfileData.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGDamageExecution)

struct FDamageStatics
{
public:
	FDamageStatics()
	{
		HeadResistancePercentDef = FGameplayEffectAttributeCaptureDefinition(UPUBGCombatSet::GetHeadResistancePercentAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
		BodyResistancePercentDef = FGameplayEffectAttributeCaptureDefinition(UPUBGCombatSet::GetBodyResistancePercentAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}

public:
	FGameplayEffectAttributeCaptureDefinition HeadResistancePercentDef;
	FGameplayEffectAttributeCaptureDefinition BodyResistancePercentDef;
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UPUBGDamageExecution::UPUBGDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().HeadResistancePercentDef);
	RelevantAttributesToCapture.Add(DamageStatics().BodyResistancePercentDef);
}

void UPUBGDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	// ����Ʈ ��������
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// �ǰ� ���� ��������
	const FGameplayEffectContextHandle& ContextHandle = Spec.GetContext();
	const FHitResult* HitResult = ContextHandle.GetHitResult();

	// ����Ʈ�� �ִ� ������ �±� ��������
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// ��� Attribute�� �Ӽ� ��������
	float HeadResistancePercent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HeadResistancePercentDef, EvaluateParameters, HeadResistancePercent);

	float BodyResistancePercent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BodyResistancePercentDef, EvaluateParameters, BodyResistancePercent);

	// ��� ASC ��������
	//UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	//UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// ASC�� ���� ��������
	//AActor* SourceActor = SourceASC ? SourceASC->GetOwner() : nullptr;
	//AActor* TargetActor = TargetASC ? TargetASC->GetOwner() : nullptr;

	// ASC�� �ƹ�Ÿ ���� ��������
	//AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	//AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	float FinalDamage = Spec.GetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_BaseDamage);

	// ������ �ǰ� ó��
	UPUBGDamageProfileData* DamageProfileData = UPUBGAssetManager::Get().GetAssetByName<UPUBGDamageProfileData>(TEXT("DamageProfileData"));

	// ���� ǥ�� ��������
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult->PhysMaterial.Get());

	TEnumAsByte<EPhysicalSurface> SurfaceByte = TEnumAsByte<EPhysicalSurface>(SurfaceType);

	FinalDamage *= DamageProfileData->GetDefaultDamageProfileRate(SurfaceByte);

	// ���� ������ ó��
	float WeaponClassTypeRawValue = Spec.GetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_EWeaponClassType);
	EWeaponClassType WeaponClassType = static_cast<EWeaponClassType>(FMath::RoundToInt(WeaponClassTypeRawValue));

	FinalDamage *= DamageProfileData->GetWeaponDamageProfileRate(WeaponClassType, SurfaceByte);

	// ���, ���� ����� �ش� ������ ���ݽ� ���� ���� ����
	switch (SurfaceType)
	{
	case SurfaceType1: // �Ӹ�
		if (TargetTags->HasTag(PUBGGameplayTags::Equip_Helmet))
		{
			FinalDamage -= FinalDamage * HeadResistancePercent;
			//TODO : ��� ������ ���� ����
		}
		break;
	case SurfaceType4: // ����
	case SurfaceType5: // ��
	case SurfaceType6: // �㸮
		if (TargetTags->HasTag(PUBGGameplayTags::Equip_Vest))
		{
			FinalDamage -= FinalDamage * BodyResistancePercent;
			// TODO : ���� ������ ���� ����
		}
		break;
	}

	// ������ ����
	if (FinalDamage > 0)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPUBGVitalSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
#endif //  WITH_SERVER_CODE
}
