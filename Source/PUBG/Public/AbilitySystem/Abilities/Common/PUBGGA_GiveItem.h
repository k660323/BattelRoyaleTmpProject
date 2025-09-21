// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "PUBGGA_GiveItem.generated.h"

class UPUBGItemTemplate;

USTRUCT(BlueprintType)
struct FPUBGGiveItemData
{
	GENERATED_BODY()

public:
	FPUBGGiveItemData() : GiveItemTemplateClass(nullptr), GiveCount(1) { }
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UPUBGItemTemplate> GiveItemTemplateClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GiveCount;
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGA_GiveItem : public UPUBGGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPUBGGA_GiveItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FPUBGGiveItemData> GiveItemDatas;
};
