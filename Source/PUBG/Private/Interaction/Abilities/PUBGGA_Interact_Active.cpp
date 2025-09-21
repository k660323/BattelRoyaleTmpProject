// Kakanu Portfolio Project


#include "Interaction/Abilities/PUBGGA_Interact_Active.h"
#include "PUBGGameplayTags.h"
#include "Characters/PUBGBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/Abilities/PUBGGA_Interact.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "Interaction/Abilities/Task/PUBGAbilityTask_CheckForInvalidIntraction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Interact_Active)

UPUBGGA_Interact_Active::UPUBGGA_Interact_Active(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::Manual;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bServerRespectsRemoteAbilityCancellation = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_Interact_Active);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Interact);

	{
		FAbilityTriggerData TirggerData;
		TirggerData.TriggerTag = PUBGGameplayTags::Ability_Interact_Active;
		TirggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TirggerData);
	}
}

void UPUBGGA_Interact_Active::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// Target액터가 인터렉션 인터페이스인지 확인 및 인터페이스 관련 데이터 설정
	if (InitInteractionData(const_cast<AActor*>(TriggerEventData->Target.Get())) == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	CachedInteractionRange = TriggerEventData->EventMagnitude;

	if (InteractionInfo.Duration <= 0.0f)
	{
		TriggerInteraction();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FlushPressedInput(MoveInputAction);

	if (APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo())
	{
		if (UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(BaseCharacter->GetCharacterMovement()))
		{
			CharacterMovement->StopMovementImmediately();
		}

		// 
	}

	FPUBGInteractionMessage Message;
	Message.Instigator = GetAvatarActorFromActorInfo();
	Message.bShouldRefresh = true;
	Message.bSwitchActive = true;
	Message.InteractionInfo = InteractionInfo;

	// UI 호출
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(PUBGGameplayTags::Message_Interaction_Progress, Message);

	// 상호작용 몽타주 실행
	if (UAnimMontage* ActiveStartMontage = InteractionInfo.ActiveStartMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("InteractMontage"), ActiveStartMontage, 1.0f, NAME_None, true, 1.0f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}

	// 상호작용 취소 조건을 확인을 위한 태스크
	if (UPUBGAbilityTask_CheckForInvalidIntraction* InvalidIntractionTask = UPUBGAbilityTask_CheckForInvalidIntraction::CheckForInvalidIntraction(this, AcceptanceAngle, AcceptanceDistance))
	{
		InvalidIntractionTask->OnInvalidIntraction.AddDynamic(this, &ThisClass::OnInvalidInteraction);
		InvalidIntractionTask->ReadyForActivation();
	}

	// 어빌리티 취소하기위한 키입력 대기 태스크
	if (UAbilityTask_WaitInputPress* InputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this))
	{
		InputPressTask->OnPress.AddDynamic(this, &ThisClass::OnInputPrssed);
		InputPressTask->ReadyForActivation();
	}

	GetWorld()->GetTimerManager().SetTimer(InteractionTimerHandle, this, &ThisClass::OnDurationEnded, InteractionInfo.Duration, false);
}

void UPUBGGA_Interact_Active::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPUBGGA_Interact_Active::TriggerInteraction()
{
	FGameplayEventData Payload;
	Payload.EventTag = PUBGGameplayTags::GameplayEvent_Interact;
	Payload.Instigator = GetAvatarActorFromActorInfo();
	Payload.Target = InteractableActor;
	Payload.EventMagnitude = CachedInteractionRange;

	Interactable->CustomizeInteractionEventData(PUBGGameplayTags::GameplayEvent_Interact, Payload);
	
	SendGameplayEvent(PUBGGameplayTags::GameplayEvent_Interact, Payload);
}

void UPUBGGA_Interact_Active::OnInputPrssed(float TimeWaited)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UPUBGGA_Interact_Active::OnDurationEnded()
{
	if (UAbilityTask_NetworkSyncPoint* NetSyncPointTask = UAbilityTask_NetworkSyncPoint::WaitNetSync(this, EAbilityTaskNetSyncType::OnlyServerWait))
	{
		NetSyncPointTask->OnSync.AddDynamic(this, &ThisClass::OnNetSync);
		NetSyncPointTask->ReadyForActivation();
	}
}

void UPUBGGA_Interact_Active::OnNetSync()
{
	TriggerInteraction();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPUBGGA_Interact_Active::OnInvalidInteraction()
{

	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}
