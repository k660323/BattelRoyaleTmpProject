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
	// 이펙트 가져오기
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 피격 정보 가져오기
	const FGameplayEffectContextHandle& ContextHandle = Spec.GetContext();
	const FHitResult* HitResult = ContextHandle.GetHitResult();

	// 이펙트에 있는 대상들의 태그 가져오기
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// 대상 Attribute의 속성 가져오기
	float HeadResistancePercent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HeadResistancePercentDef, EvaluateParameters, HeadResistancePercent);

	float BodyResistancePercent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BodyResistancePercentDef, EvaluateParameters, BodyResistancePercent);

	// 대상 ASC 가져오기
	//UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	//UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// ASC의 액터 가져오기
	//AActor* SourceActor = SourceASC ? SourceASC->GetOwner() : nullptr;
	//AActor* TargetActor = TargetASC ? TargetASC->GetOwner() : nullptr;

	// ASC의 아바타 액터 가져오기
	//AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	//AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	float FinalDamage = Spec.GetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_BaseDamage);

	// 부위별 피격 처리
	UPUBGDamageProfileData* DamageProfileData = UPUBGAssetManager::Get().GetAssetByName<UPUBGDamageProfileData>(TEXT("DamageProfileData"));

	// 물리 표면 가져오기
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult->PhysMaterial.Get());

	TEnumAsByte<EPhysicalSurface> SurfaceByte = TEnumAsByte<EPhysicalSurface>(SurfaceType);

	FinalDamage *= DamageProfileData->GetDefaultDamageProfileRate(SurfaceByte);

	// 무기 데미지 처리
	float WeaponClassTypeRawValue = Spec.GetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_EWeaponClassType);
	EWeaponClassType WeaponClassType = static_cast<EWeaponClassType>(FMath::RoundToInt(WeaponClassTypeRawValue));

	FinalDamage *= DamageProfileData->GetWeaponDamageProfileRate(WeaponClassType, SurfaceByte);

	// 헬멧, 조끼 존재시 해당 부위에 직격시 피해 감소 적용
	switch (SurfaceType)
	{
	case SurfaceType1: // 머리
		if (TargetTags->HasTag(PUBGGameplayTags::Equip_Helmet))
		{
			FinalDamage -= FinalDamage * HeadResistancePercent;
			//TODO : 헬멧 내구도 감소 구현
		}
		break;
	case SurfaceType4: // 가슴
	case SurfaceType5: // 배
	case SurfaceType6: // 허리
		if (TargetTags->HasTag(PUBGGameplayTags::Equip_Vest))
		{
			FinalDamage -= FinalDamage * BodyResistancePercent;
			// TODO : 조끼 내구도 감소 구현
		}
		break;
	}

	// 데미지 적용
	if (FinalDamage > 0)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPUBGVitalSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
#endif //  WITH_SERVER_CODE
}
