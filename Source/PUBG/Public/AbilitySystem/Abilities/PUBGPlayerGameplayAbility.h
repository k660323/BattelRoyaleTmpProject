// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGPlayerGameplayAbility.generated.h"

class APUBGPlayerCharacter;
class APUBGPlayerController;

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerGameplayAbility : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "PUBG|Ability")
	APUBGPlayerCharacter* GetPlayerCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "PUBG|Ability")
	APUBGPlayerController* GetPlayerControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "PUBG|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectsSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);

	UFUNCTION(BlueprintPure, Category = "PUBG|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime);

protected:
	TWeakObjectPtr<APUBGPlayerCharacter> CachedPUBGPlayerCharacter;
	TWeakObjectPtr<APUBGPlayerController> CachedPUBGPlayerController;

};
