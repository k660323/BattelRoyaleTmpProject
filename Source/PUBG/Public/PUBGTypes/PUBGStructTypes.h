// Kakanu Portfolio Project

#pragma once

#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "Engine/DataTable.h"
#include "PUBGDefine.h"
#include "PUBGStructTypes.generated.h"

class UInputMappingContext;
class UGameplayEffect;
class UAttributeSet;
class UPUBGGameplayAbility;

USTRUCT(BlueprintType)
struct FPUBGAbilitySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UPUBGGameplayAbility> AbilityToGrant;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

public:
	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FPUBGGameplayTagAbilitySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = " InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UPUBGGameplayAbility> AbilityToGrant;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

public:
	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FPUBGGameplayEffectSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;

public:
	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FPUBGAttributeSetSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet = nullptr;

public:
	bool IsValid() const;
};
