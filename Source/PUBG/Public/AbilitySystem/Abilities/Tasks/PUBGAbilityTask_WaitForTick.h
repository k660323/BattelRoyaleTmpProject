// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PUBGAbilityTask_WaitForTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAbilityTask_WaitForTick : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPUBGAbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPUBGAbilityTask_WaitForTick* WaitForTick(UGameplayAbility* OwningAbility);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;
};
