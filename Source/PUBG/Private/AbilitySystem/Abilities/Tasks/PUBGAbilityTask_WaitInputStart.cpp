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
		// �̺�Ʈ ��� (����, Ŭ��)
		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UPUBGAbilityTask_WaitInputStart::OnStartCallback);
		// ���� Ŭ���� (����)
		if (IsForRemoteClient())
		{
			// Ŭ��κ��� ������ �̺�Ʈ�� �Դ��� üũ
			if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()) == false)
			{
				// ������ ��� 
				// (�÷��̾� �����͸� ���������� ������ EndTask()�� ������� �ʰ�, �̺�Ʈ ���Ŵ��)
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

	// �̺�Ʈ ����
	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	// ���� Ŭ��
	if (IsPredictingClient())
	{
		//  �������� ���� �̺�Ʈ ����
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		// �̺�Ʈ �Һ�
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnStart.Broadcast();
	}
	EndTask();
}
