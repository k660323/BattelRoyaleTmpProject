// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/Player/PUBGGA_Equipment.h"
#include "PUBGGA_Weapon_Melee.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Weapon_Melee : public UPUBGGA_Equipment
{
	GENERATED_BODY()
	
public:
	UPUBGGA_Weapon_Melee(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// 피격 액터 셋 초기화 (중복 체크하는 배열)
	UFUNCTION()
	void ResetHitActors();

	// 피격 액터를 배열에 넣는다.
	// APUBGBaseCharacer 클래스 위주로 캐스팅하여 넣을려고한다.
	// 캐스팅이 안되면 그냥 액터를 넣는다.
	UFUNCTION()
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutHitIndexes);

	// Cue 생성 및 피격 대상 Effect 처리
	UFUNCTION()
	void ProcessHitResult(FHitResult HitResult, float Damage, APUBGEquipmentBase* WeaponActor);

	UFUNCTION()
	void DrawDebugHitPoint(const FHitResult& HitResult);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "D1|Melee")
	bool bShowDebug = false;
	
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CachedHitActors;
};
