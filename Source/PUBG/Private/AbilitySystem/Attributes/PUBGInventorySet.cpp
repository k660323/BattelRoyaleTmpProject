// Kakanu Portfolio Project


#include "AbilitySystem/Attributes/PUBGInventorySet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "PUBGGameplayTags.h"
#include "Messages/PUBGAdditionalValueMessage.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGInventorySet)

UPUBGInventorySet::UPUBGInventorySet()
{
}

void UPUBGInventorySet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AdditionalMaxCapacityWeight, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UPUBGInventorySet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetAdditionalMaxCapacityWeightAttribute())
	{
		FPUBGAdditionalValueMessage Message;
		Message.EventTag = PUBGGameplayTags::Message_AdditionalValue;
		Message.ChangedAdditionalValue = GetAdditionalMaxCapacityWeight();

		// ��ε� ĳ��Ʈ
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSystem.BroadcastMessage(Message.EventTag, Message);
	}
}

void UPUBGInventorySet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UPUBGInventorySet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);

}

void UPUBGInventorySet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetAdditionalMaxCapacityWeightAttribute())
	{
		if (GetOwningActor()->HasAuthority())
		{
			FPUBGAdditionalValueMessage Message;
			Message.EventTag = PUBGGameplayTags::Message_AdditionalValue;
			Message.ChangedAdditionalValue = NewValue;

			// ��ε� ĳ��Ʈ
			if (UWorld* World = GetWorld())
			{
				UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(World);
				MessageSystem.BroadcastMessage(Message.EventTag, Message);
			}
		}
	}
}

void UPUBGInventorySet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetAdditionalMaxCapacityWeightAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1000.0f);
	}
}

void UPUBGInventorySet::OnRep_AdditionalMaxCapacityWeight(const FGameplayAttributeData& OldValue)
{
	// ���޹��� ���� ASC�� �˸� (PostAttributeChange�Լ� Ŭ�󿡼��� ����ȣ��, Base,Current �������� �ι� ȣ��ȴ�)
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AdditionalMaxCapacityWeight, OldValue);
	
	FPUBGAdditionalValueMessage Message;
	Message.EventTag = PUBGGameplayTags::Message_AdditionalValue;
	Message.ChangedAdditionalValue = GetAdditionalMaxCapacityWeight();
	
	// ��ε� ĳ��Ʈ
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(Message.EventTag, Message);
}