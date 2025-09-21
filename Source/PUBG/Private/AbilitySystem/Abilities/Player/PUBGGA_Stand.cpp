// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Player/PUBGGA_Stand.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Stand)

UPUBGGA_Stand::UPUBGGA_Stand(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnSpawn;
	
	AbilityTags.AddTag(PUBGGameplayTags::Ability_Stand);
	ActivationOwnedTags.AddTag(PUBGGameplayTags::Status_Stand);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		TriggerData.TriggerTag = PUBGGameplayTags::GameplayEvent_Stand;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPUBGGA_Stand::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AActor* Actor = GetAvatarActorFromActorInfo())
	{
		if (UPUBGActiveEquipmentManagerComponent* ActiveEquipmentManager = Actor->FindComponentByClass<UPUBGActiveEquipmentManagerComponent>())
		{
			ActiveEquipmentManager->RefreshCurActiveEuipmentAttachSocket();
		}
	}
}

void UPUBGGA_Stand::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
