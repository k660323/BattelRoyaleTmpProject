#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "PUBGAbilityTask_CheckForInvalidIntraction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvalidIntraction);

UCLASS()
class UPUBGAbilityTask_CheckForInvalidIntraction : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPUBGAbilityTask_CheckForInvalidIntraction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UPUBGAbilityTask_CheckForInvalidIntraction* CheckForInvalidIntraction(UGameplayAbility* OwningAbility, float AcceptanceAngle, float AcceptanceDistance);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	UFUNCTION()
	void CheckForInvalidIntraction();

	UFUNCTION()
	float CalculateAngle2D();

public:
	UPROPERTY(BlueprintAssignable)
	FOnInvalidIntraction OnInvalidIntraction;

private:
	float AcceptanceAngle;
	float AcceptanceDistance;

	FVector CachedDirection2D;
	FVector CachedActorLocation;

	FTimerHandle CheckTimerHandle;
};

