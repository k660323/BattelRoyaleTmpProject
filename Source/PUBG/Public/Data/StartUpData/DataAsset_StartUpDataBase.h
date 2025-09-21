// Kakanu Portfolio Project

#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "PUBGTypes/PUBGStructTypes.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UPUBGAbilitySystemComponent;
class UPUBGGameplayAbility;
class UAttributeSet;

USTRUCT(BlueprintType)
struct FPUBGStartUpData_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& InHandle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& InHandle);
	void AddAttributeSet(UAttributeSet* InAttributeSet);

	void ClearByAbilitySystem(UPUBGAbilitySystemComponent* ASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectSpecHandles;
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * 
 */
UCLASS(BlueprintType, Const)
class PUBG_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void GiveToAbilitySystemComponent(UPUBGAbilitySystemComponent* InASCToGive, FPUBGStartUpData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;

protected:
	//void GrantAbilities(const TArray<TSubclassOf<UPUBGGameplayAbility>>& InAbilitiesToGive, UPUBGAbilitySystemComponent* InASCToGive) const;
	void GrantAbilities(const TArray<FPUBGAbilitySet>& InAbilitiesToGive, UPUBGAbilitySystemComponent* InASCToGive, FPUBGStartUpData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;
	void GrantAbilities(const TArray<FPUBGGameplayTagAbilitySet>& InAbilitiesToGive, UPUBGAbilitySystemComponent* InASCToGive, FPUBGStartUpData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;

protected:
	// 일회용 어빌리티 (어빌리티 끝나면 어빌리티 제거)
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAbilitySet> ActivateOnGiveAbilities;

	// 이벤트 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAbilitySet> ReactiveAbilities;

	// 시작 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGGameplayEffectSet> StartUpGameplayEffects;

	// 시작 어트리뷰트
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAttributeSetSet> StartUpAttributes;

	// 상시 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAbilitySet> ActivateOnSpawnAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGGameplayTagAbilitySet> ActivateOnSpawnAbilitySets;

	// 동적 어빌리티
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FPUBGGameplayTagAbilitySet> StartAbilitySets;
};
