// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Weapon/PUBGGA_Weapon_Melee.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "System/PUBGAssetManager.h"
#include "Actors/PUBGEquipmentBase.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Weapon_Melee)

UPUBGGA_Weapon_Melee::UPUBGGA_Weapon_Melee(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UPUBGGA_Weapon_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ResetHitActors();
}

void UPUBGGA_Weapon_Melee::ResetHitActors()
{
	CachedHitActors.Reset();
}

void UPUBGGA_Weapon_Melee::ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutHitIndexes)
{
	for (int32 i = 0; i < InTargetDataHandle.Data.Num(); i++)
	{
		const TSharedPtr<FGameplayAbilityTargetData>& TargetData = InTargetDataHandle.Data[i];

		if (FHitResult* HitResult = const_cast<FHitResult*>(TargetData->GetHitResult()))
		{
			if (AActor* HitActor = HitResult->GetActor())
			{
				APUBGBaseCharacter* TargetCharacter = Cast<APUBGBaseCharacter>(HitActor);

				if (TargetCharacter == nullptr)
				{
					TargetCharacter = Cast<APUBGBaseCharacter>(HitActor->GetOwner());
				}

				/*ID1TeamAgentInterface* TeamAgentInterface = Cast<ID1TeamAgentInterface>(TargetCharacter);
				if (TeamAgentInterface && (TeamAgentInterface->GetTeamAttitudeTowards(*GetLyraCharacterFromActorInfo()) != ETeamAttitude::Hostile))
				{
					continue;
				}*/

				AActor* SelectedActor = TargetCharacter ? TargetCharacter : HitActor;

				if (CachedHitActors.Contains(SelectedActor))
					continue;

				CachedHitActors.Add(SelectedActor);

				OutHitIndexes.Add(i);
			}
		}

	}
}

void UPUBGGA_Weapon_Melee::ProcessHitResult(FHitResult HitResult, float Damage, APUBGEquipmentBase* WeaponActor)
{
	UPUBGAbilitySystemComponent* SourceASC = GetPUBGAbilitySystemComponentFromActorInfo();

	if (SourceASC == nullptr)
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(SourceASC, GetCurrentActivationInfo().GetActivationPredictionKey());

	FGameplayCueParameters CueParameters;
	CueParameters.Location = HitResult.ImpactPoint;
	CueParameters.Normal = HitResult.ImpactNormal;
	CueParameters.PhysicalMaterial = HitResult.PhysMaterial.Get();
	// SourceASC->ExecuteGameplayCue(PUBGGameplayTags::GameplayCue_Weapon_Impact, CueParameters);

	if (HasAuthority(&CurrentActivationInfo))
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor());
		const TSubclassOf<UGameplayEffect> DamageEffectClass = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("DamageGameplayEffect"));
		// 적용시킬 GameplayEffect 정보
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

		// 적용 시킬 대상
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		EffectContext.AddHitResult(HitResult);

		// ASC 데이터를 직접 소유하는 액터 (EX: PlayerState)
		EffectContext.AddInstigator(SourceASC->AbilityActorInfo->OwnerActor.Get(), GetOwningActorFromActorInfo());

		// SpecHandle에 ContextHandle 추가
		EffectSpecHandle.Data->SetContext(EffectContext);
		
		if (WeaponActor)
		{
			EWeaponClassType WeaponClassType = WeaponActor->GetWeaponClassType();
			float WeaponClassTypeRawValue = static_cast<float>(WeaponClassType);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_EWeaponClassType, WeaponClassTypeRawValue);
		}

		EffectSpecHandle.Data->SetSetByCallerMagnitude(PUBGGameplayTags::SetByCaller_BaseDamage, Damage);
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
	}

	DrawDebugHitPoint(HitResult);
}

void UPUBGGA_Weapon_Melee::DrawDebugHitPoint(const FHitResult& HitResult)
{
#if UE_EDITOR
	if (GIsEditor)
	{
		if (bShowDebug)
		{
			FColor Color = HasAuthority(&CurrentActivationInfo) ? FColor::Red : FColor::Green;
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 4, 32, Color, false, 5);
		}
	}
#endif // UE_EDITOR
}
