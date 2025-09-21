// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_TogglePressed.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilityTask_TogglePressed)

UPUBGAbilityTask_TogglePressed::UPUBGAbilityTask_TogglePressed(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void UPUBGAbilityTask_TogglePressed::OnDestroy(bool bInOwnerFinished)
{
    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (ASC && DelegateHandle.IsValid())
    {
        ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
    }

    Super::OnDestroy(bInOwnerFinished);
}

UPUBGAbilityTask_TogglePressed* UPUBGAbilityTask_TogglePressed::TogglePressed(UGameplayAbility* OwningAbility)
{
    UPUBGAbilityTask_TogglePressed* Task = NewAbilityTask<UPUBGAbilityTask_TogglePressed>(OwningAbility);
    return Task;
}

void UPUBGAbilityTask_TogglePressed::Activate()
{
    GetWorld()->GetTimerManager().SetTimerForNextTick([this](){

        UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

        if (ASC && Ability)
        {
            // ����/Ŭ���̾�Ʈ ���ʿ��� InputPressed �̺�Ʈ ������
            DelegateHandle = ASC->AbilityReplicatedEventDelegate(
                EAbilityGenericReplicatedEvent::InputPressed,
                GetAbilitySpecHandle(), GetActivationPredictionKey()
            ).AddUObject(this, &UPUBGAbilityTask_TogglePressed::OnTogglePressedCallback);

            if (IsForRemoteClient())
            {
                if (ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()) == false)
                {
                    SetWaitingOnRemotePlayerData();
                }
            }
        }
    });
}

void UPUBGAbilityTask_TogglePressed::OnTogglePressedCallback()
{
    UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
    if (Ability == nullptr || ASC == nullptr)
        return;

    // ��Ÿ�� üũ
    if (Ability->K2_CheckAbilityCooldown() == false)
    {
        return;
    }

    // ��� ���� ���� üũ
    if (OnToggleReleaseCondition.IsBound() && OnToggleReleaseCondition.Execute() == false)
    {
        return;
    }

    FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

    // Ŭ�󿡼� �̺�Ʈ ������ ����
    if (IsPredictingClient())
    {
        ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
    }
    // ������ ����Ű ����
    else
    {
        ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey());
    }

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnTogglePressed.Broadcast();
    }
}