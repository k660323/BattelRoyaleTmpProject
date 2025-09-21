// Kakanu Portfolio Project


#include "Interaction/Abilities/PUBGGA_Interact_Info_Object.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction//Abilities/Task/PUBGAbilityTask_CheckForInvalidIntraction.h"
#include "PUBGGameplayTags.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Interact_Info_Object)

UPUBGGA_Interact_Info_Object::UPUBGGA_Interact_Info_Object(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_Interact_Object);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Interact);

	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_Interact;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPUBGGA_Interact_Info_Object::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	if (InitInteractionData(const_cast<AActor*>(TriggerEventData->Target.Get())) == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	FPUBGInteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
	InteractionQuery.RequestingController = GetControllerFromActorInfo();

	if (Interactable->CanInteraction(InteractionQuery) == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// 월드 상호작용 액터 오브젝트


	// 상호작용 취소 체크
	if (UPUBGAbilityTask_CheckForInvalidIntraction* InvalidIntractionTask = UPUBGAbilityTask_CheckForInvalidIntraction::CheckForInvalidIntraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidIntractionTask->OnInvalidIntraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidIntractionTask->ReadyForActivation();
	}

	if (UAnimMontage* EndAnimMontage = InteractionInfo.ActiveEndMontage)
	{
		if (UAbilityTask_PlayMontageAndWait * MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractionMontage"), EndAnimMontage, 1.0f, NAME_None, true, 1.0f, 0.0f, false))
		{
			MontageTask->ReadyForActivation();
		}
	}
}

void UPUBGGA_Interact_Info_Object::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// TEMP
	// 인터렉션 Duration 이 0.0f 초과하는 값이면 무기를 넣는 애니메이션이 발생하였으므로
	// 무기를 다시 꺼내는 애니메이션을 실행한다.
	if (InteractionInfo.Duration > 0.0f)
	{
		if (APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo())
		{
			if (UPUBGActiveEquipmentManagerComponent* EquipmentManagerComponent = BaseCharacter->GetComponentByClass<UPUBGActiveEquipmentManagerComponent>())
			{

			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UPUBGGA_Interact_Info_Object::OnInvalidInteraction()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
