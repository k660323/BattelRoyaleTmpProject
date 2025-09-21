// Kakanu Portfolio Project


#include "Interaction/Abilities/PUBGGA_Interact_Info_Pickup.h"
#include "Actors/PUBGPickupableItemBase.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGItemManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Physics/PUBGPhysicsFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Interact_Info_Pickup)

UPUBGGA_Interact_Info_Pickup::UPUBGGA_Interact_Info_Pickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::Manual;

	// 클라 요청 무시
	bServerRespectsRemoteAbilityCancellation = false;
	// 보안정책 클라 또는 서버 (어빌리티 실행 조건)
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UPUBGGA_Interact_Info_Pickup::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// 상호작용 몽타주 실행
	if (UAnimMontage* ActiveStartMontage = InteractionInfo.ActiveStartMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UpperBody"), ActiveStartMontage, 1.0f, NAME_None, false, 1.0f, 0.f, false))
		{
			PlayMontageTask->ReadyForActivation();
		}
	}

	if (HasAuthority(&ActivationInfo) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UPUBGItemManagerComponent* ItemManagerComponent = GetControllerFromActorInfo()->GetComponentByClass<UPUBGItemManagerComponent>();
	if (ItemManagerComponent == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	APUBGPickupableItemBase* PickupableActor = Cast<APUBGPickupableItemBase>(InteractableActor);
	if (PickupableActor == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// 한번더 체크 (원 반지름과 박스 컴포넌트 1:1 비교) (최적화)
	FVector SphereCenter = GetAvatarActorFromActorInfo()->GetActorLocation();
	FBox Box = PickupableActor->GetCollisionBoxComponent()->Bounds.GetBox();
	float Radius = TriggerEventData->EventMagnitude + 10.0f; // 10 보정
	bool bHit = UPUBGPhysicsFunctionLibrary::SphereIntersectsBox(SphereCenter, Radius, Box);

	if (bHit == false)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (ItemManagerComponent->TryPickupItem(PickupableActor))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}
