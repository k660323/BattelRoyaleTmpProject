// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitInputStart.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilityTask_WaitInputStart)

UPUBGAbilityTask_WaitInputStart::UPUBGAbilityTask_WaitInputStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UPUBGAbilityTask_WaitInputStart* UPUBGAbilityTask_WaitInputStart::WaitInputStart(UGameplayAbility* OwningAbility)
{
	UPUBGAbilityTask_WaitInputStart* Task = NewAbilityTask< UPUBGAbilityTask_WaitInputStart>(OwningAbility);
	return Task;
}

void UPUBGAbilityTask_WaitInputStart::Activate()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		// 이벤트 등록 (서버, 클라)
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UPUBGAbilityTask_WaitInputStart::OnStartCallback);
		// 원격 클라라면 (서버)
		if (IsForRemoteClient())
		{
			// 클라로부터 먼저온 이벤트가 왔는지 체크
			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()) == false)
			{
				// 없으면 대기 
				// (플레이어 데이터를 받을때까지 강제로 EndTask()가 실행되지 않고, 이벤트 수신대기)
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UPUBGAbilityTask_WaitInputStart::OnStartCallback()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (Ability == nullptr || ASC == nullptr)
		return;

	// 이벤트 제거
	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	// 예측 클라
	if (IsPredictingClient())
	{
		//  서버에게 예측 이벤트 전송
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		// 이벤트 소비
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStart.Broadcast();
	}
	EndTask();
}
