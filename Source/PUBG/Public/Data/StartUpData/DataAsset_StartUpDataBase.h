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
	// ��ȸ�� �����Ƽ (�����Ƽ ������ �����Ƽ ����)
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAbilitySet> ActivateOnGiveAbilities;

	// �̺�Ʈ �����Ƽ
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAbilitySet> ReactiveAbilities;

	// ���� ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGGameplayEffectSet> StartUpGameplayEffects;

	// ���� ��Ʈ����Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAttributeSetSet> StartUpAttributes;

	// ��� �����Ƽ
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGAbilitySet> ActivateOnSpawnAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray<FPUBGGameplayTagAbilitySet> ActivateOnSpawnAbilitySets;

	// ���� �����Ƽ
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FPUBGGameplayTagAbilitySet> StartAbilitySets;
};
