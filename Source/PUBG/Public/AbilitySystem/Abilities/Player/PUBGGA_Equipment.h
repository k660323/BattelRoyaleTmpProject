// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Item/PUBGItemTemplate.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_Equipment.generated.h"

class APUBGEquipmentBase;
class UPUBGItemInstance;

USTRUCT(BlueprintType)
struct FPUBGActiveEquipmentInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "PUBG|ActiveEquipment")
	EEquipmentType EquipmentType = EEquipmentType::Count;

public:
	UPROPERTY(EditAnywhere, Category = "PUBG|Equipment", meta = (EditCondition = "EquipmentType==EEquipmentType::Armor", EditConditionHides))
	EArmorType RequiredArmorType = EArmorType::Count;

public:
	UPROPERTY(EditAnywhere, Category = "PUBG|Equipment", meta = (EditCondition = "EquipmentType==EEquipmentType::Weapon", EditConditionHides))
	EWeaponType RequiredWeaponType = EWeaponType::Count;

public:
	UPROPERTY(EditAnywhere, Category = "PUBG|Equipment", meta = (EditCondition = "EquipmentType==EEquipmentType::Utility", EditConditionHides))
	EUtilityType RequiredUtilityType = EUtilityType::Count;

public:
	UPROPERTY(EditAnywhere, Category = "PUBG|Equipment", meta = (EditCondition = "EquipmentType==EEquipmentType::Fashion", EditConditionHides))
	EFashionType RequiredFashionType = EFashionType::Count;

public:
	UPROPERTY()
	TWeakObjectPtr<APUBGEquipmentBase> EquipmentActor;
};
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_Equipment : public UPUBGGameplayAbility
{
	GENERATED_BODY()
public:
	UPUBGGA_Equipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	APUBGEquipmentBase* GetEquipmentActor() const;
	UPUBGItemInstance* GetEquipmentItemInstance(const APUBGEquipmentBase* InEquipmentActor) const;

	float GetEquipmentStatValue(FGameplayTag InStatTag, const APUBGEquipmentBase* InEquipmentActor) const;
	FORCEINLINE float GetCachedAttackRate() const { return CachedAttackRate;};

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PUBG|ActiveEquipment")
	FPUBGActiveEquipmentInfo ActiveEquipmentInfo;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|ActiveEquipment")
	float DefaultAttackRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "PUBG|ActiveEquipment", meta = (ForceInlineRow))
	TArray<FStatSet> StatSets;

private:
	float CachedAttackRate = 0.f;

	UPROPERTY()
	FActiveGameplayEffectHandle GameplayEffectHandle;
};
