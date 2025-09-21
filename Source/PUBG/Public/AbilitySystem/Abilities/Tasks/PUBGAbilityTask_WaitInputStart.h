// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PUBGAbilityTask_WaitInputStart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputStartDelegate);
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAbilityTask_WaitInputStart : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UPUBGAbilityTask_WaitInputStart(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPUBGAbilityTask_WaitInputStart* WaitInputStart(UGameplayAbility* OwningAbility);

public:
	virtual void Activate() override;

public:
	UFUNCTION()
	void OnStartCallback();

public:
	UPROPERTY(BlueprintAssignable)
	FInputStartDelegate OnStart;

protected:
	FDelegateHandle DelegateHandle;
};
