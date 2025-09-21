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
	// �ǰ� ���� �� �ʱ�ȭ (�ߺ� üũ�ϴ� �迭)
	UFUNCTION()
	void ResetHitActors();

	// �ǰ� ���͸� �迭�� �ִ´�.
	// APUBGBaseCharacer Ŭ���� ���ַ� ĳ�����Ͽ� ���������Ѵ�.
	// ĳ������ �ȵǸ� �׳� ���͸� �ִ´�.
	UFUNCTION()
	void ParseTargetData(const FGameplayAbilityTargetDataHandle& InTargetDataHandle, TArray<int32>& OutHitIndexes);

	// Cue ���� �� �ǰ� ��� Effect ó��
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
