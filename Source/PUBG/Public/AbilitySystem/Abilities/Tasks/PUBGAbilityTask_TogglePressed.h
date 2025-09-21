// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PUBGAbilityTask_TogglePressed.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTogglePressedDelegate);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FToggleReleaseConditionDelegate);

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAbilityTask_TogglePressed : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UPUBGAbilityTask_TogglePressed(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPUBGAbilityTask_TogglePressed* TogglePressed(UGameplayAbility* OwningAbility);

public:
	virtual void Activate() override;

public:
	UFUNCTION()
	void OnTogglePressedCallback();

public:
	UPROPERTY(BlueprintAssignable)
	FTogglePressedDelegate OnTogglePressed;

	UPROPERTY()
	FToggleReleaseConditionDelegate OnToggleReleaseCondition;

protected:
	FDelegateHandle DelegateHandle;
};
