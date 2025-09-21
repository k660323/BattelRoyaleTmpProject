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

	// Ŭ�� ��û ����
	bServerRespectsRemoteAbilityCancellation = false;
	// ������å Ŭ�� �Ǵ� ���� (�����Ƽ ���� ����)
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

	// ��ȣ�ۿ� ��Ÿ�� ����
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

	// �ѹ��� üũ (�� �������� �ڽ� ������Ʈ 1:1 ��) (����ȭ)
	FVector SphereCenter = GetAvatarActorFromActorInfo()->GetActorLocation();
	FBox Box = PickupableActor->GetCollisionBoxComponent()->Bounds.GetBox();
	float Radius = TriggerEventData->EventMagnitude + 10.0f; // 10 ����
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
