// Kakanu Portfolio Project


#include "Interaction/Abilities/PUBGGA_Interact.h"
#include "Interaction/PUBGInteractionQuery.h"
#include "Interaction/Abilities/Task/PUBGAbilityTask_WaitForInteractableTraceHit.h"
#include "Interaction/Abilities/Task/PUBGAbilityTask_WaitForInteractableSphereTrace.h"
#include "Physics/PUBGCollisionChannels.h"
#include "Interaction/PUBGInteractionInfo.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "PUBGGameplayTags.h"
#include "AbilitySystem/Abilities/Tasks/PUBGAbilityTask_WaitInputStart.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Characters/PUBGBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Interact)

UPUBGGA_Interact::UPUBGGA_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnSpawn;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UPUBGGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 상호작용 요청 + 정보
	FPUBGInteractionQuery InteractionQuery;
	InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
	InteractionQuery.RequestingController = GetControllerFromActorInfo();

	if (UPUBGAbilityTask_WaitForInteractableTraceHit* WaitForInteractableTraceHitTask = UPUBGAbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(
		this, InteractionQuery, PUBG_TraceChannel_Interaction, MakeTargetLocationInfoFromOwnerActor(), InteractionTraceRange, InteractionLineTraceRate, bShowTraceDebug))
	{
		WaitForInteractableTraceHitTask->InteractableChanged.AddDynamic(this, &ThisClass::UpdateInteractions);
		WaitForInteractableTraceHitTask->ReadyForActivation();
	}

	if (IsLocallyControlled())
	{
		if (UPUBGAbilityTask_WaitForInteractableSphereTrace* WaitForInteractableSphereTrace = UPUBGAbilityTask_WaitForInteractableSphereTrace::WaitForInteractableSphereTrace(
			this, InteractionQuery, PUBG_TraceChannel_Interaction, MakeTargetLocationInfoFromOwnerActor(), InteractionTraceRange, InteractionSphereTraceRate, bShowTraceDebug))
		{
			WaitForInteractableSphereTrace->NearInteractableChanged.AddDynamic(this, &ThisClass::UpdateNearInteractions);
			WaitForInteractableSphereTrace->ReadyForActivation();
		}
	}

	WaitInputStart();
}

void UPUBGGA_Interact::UpdateInteractions(const TArray<FPUBGInteractionInfo>& InteractionInfos)
{
	FPUBGInteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.bSwitchActive = (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(PUBGGameplayTags::Status_Interact) == false);
	Message.InteractionInfo = InteractionInfos.Num() > 0 ? InteractionInfos[0] : FPUBGInteractionInfo();

	// 콜백 함수 호출 (UI)
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetAvatarActorFromActorInfo());
	MessageSystem.BroadcastMessage(PUBGGameplayTags::Message_Interaction_Notice, Message);

	CurrentInteractionInfos = InteractionInfos;
}

void UPUBGGA_Interact::UpdateNearInteractions(const TArray<FPUBGInteractionInfo>& InteractionInfos)
{
	FPUBGInteractionsMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.InteractionInfos = InteractionInfos;

	// 콜백 함수 호출 (UI)
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetAvatarActorFromActorInfo());
	MessageSystem.BroadcastMessage(PUBGGameplayTags::Message_Interaction_Detect, Message);

	CurrentNearInteractionInfos = InteractionInfos;
}

void UPUBGGA_Interact::TriggerInteraction()
{
	if (CurrentInteractionInfos.Num() == 0)
	{
		return;
	}

	APUBGBaseCharacter* PUBGCharacter = GetBaseCharacterFromActorInfo();
	if (PUBGCharacter && PUBGCharacter->GetMovementComponent()->IsFalling())
	{
		return;
	}

	if (GetAbilitySystemComponentFromActorInfo())
	{
		const FPUBGInteractionInfo& InteractionInfo = CurrentInteractionInfos[0];

		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableActor = nullptr;

		if(UObject* Object = InteractionInfo.Interactable.GetObject())
		{
			if (AActor* Actor = Cast<AActor>(Object))
			{
				InteractableActor = Actor;
			}
			else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
			{
				InteractableActor = ActorComponent->GetOwner();
			}
		}

		FGameplayEventData Payload;
		Payload.EventTag = PUBGGameplayTags::Ability_Interact_Active;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableActor;
		Payload.EventMagnitude = InteractionTraceRange;

		SendGameplayEvent(PUBGGameplayTags::Ability_Interact_Active, Payload);
	}

}

void UPUBGGA_Interact::WaitInputStart()
{
	if (UPUBGAbilityTask_WaitInputStart* InputStartTask = UPUBGAbilityTask_WaitInputStart::WaitInputStart(this))
	{
		InputStartTask->OnStart.AddDynamic(this, &ThisClass::OnInputStart);
		InputStartTask->ReadyForActivation();
	}
}

void UPUBGGA_Interact::OnInputStart()
{
	TriggerInteraction();
	WaitInputStart();
}

float UPUBGGA_Interact::GetInteractionTraceRange()
{
	return InteractionTraceRange;
}
