// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Weapon/PUBGGA_AimDownSightMode.h"
#include "GameFramework/Controller.h"
#include "System/PUBGAssetManager.h"
#include "Camera/CameraSpringArmTimelineComponent.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_AimDownSightMode)

UPUBGGA_AimDownSightMode::UPUBGGA_AimDownSightMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::Manual;

	AbilityTags.AddTag(PUBGGameplayTags::Ability_AimDownSightMode);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Aiming);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Aim_ADS);
}


bool UPUBGGA_AimDownSightMode::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UPUBGGA_AimDownSightMode::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AController* Controller = GetControllerFromActorInfo();
	check(Controller);

	UCameraSpringArmTimelineComponent* CSATimelineComponent = Controller->FindComponentByClass<UCameraSpringArmTimelineComponent>();
	check(CSATimelineComponent);

	AActor* Actor = GetAvatarActorFromActorInfo();
	check(Actor);

	CSATimelineComponent->PlayTimeline(InADSCameraMode, 1);

	if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Actor->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
	{
		ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
	}
}

void UPUBGGA_AimDownSightMode::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AController* Controller = GetControllerFromActorInfo();
	check(Controller);

	UCameraSpringArmTimelineComponent* CSATimelineComponent = Controller->FindComponentByClass<UCameraSpringArmTimelineComponent>();
	check(CSATimelineComponent);

	AActor* Actor = GetAvatarActorFromActorInfo();
	check(Actor);

	CSATimelineComponent->PlayTimeline(OutADSCameraMode, 1);

	if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Actor->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
	{
		ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
	}
}