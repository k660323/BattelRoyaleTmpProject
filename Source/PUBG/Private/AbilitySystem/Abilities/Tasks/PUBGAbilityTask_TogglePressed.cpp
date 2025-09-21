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
            // 서버/클라이언트 양쪽에서 InputPressed 이벤트 리스닝
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

    // 쿨타임 체크
    if (Ability->K2_CheckAbilityCooldown() == false)
    {
        return;
    }

    // 토글 해제 조건 체크
    if (OnToggleReleaseCondition.IsBound() && OnToggleReleaseCondition.Execute() == false)
    {
        return;
    }

    FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

    // 클라에서 이벤트 서버로 전파
    if (IsPredictingClient())
    {
        ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
    }
    // 서버가 예측키 검증
    else
    {
        ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey());
    }

    if (ShouldBroadcastAbilityTaskDelegates())
    {
        OnTogglePressed.Broadcast();
    }
}