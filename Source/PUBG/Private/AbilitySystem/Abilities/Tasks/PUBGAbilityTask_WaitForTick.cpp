// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitForTick.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilityTask_WaitForTick)

UPUBGAbilityTask_WaitForTick::UPUBGAbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UPUBGAbilityTask_WaitForTick* UPUBGAbilityTask_WaitForTick::WaitForTick(UGameplayAbility* OwningAbility)
{
	UPUBGAbilityTask_WaitForTick* Task = NewAbilityTask<UPUBGAbilityTask_WaitForTick>(OwningAbility);
	return Task;
}

void UPUBGAbilityTask_WaitForTick::Activate()
{
	Super::Activate();
}

void UPUBGAbilityTask_WaitForTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
