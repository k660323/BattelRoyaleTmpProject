// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/Common/PUBGGA_GiveItem.h"
#include "Item/PUBGItemInstance.h"
#include "Item/PUBGItemTemplate.h"
#include "Item/Managers/PUBGItemManagerComponent.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Data/PUBGItemData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_GiveItem)

UPUBGGA_GiveItem::UPUBGGA_GiveItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnGiven;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
}

void UPUBGGA_GiveItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UPUBGItemManagerComponent* ItemManagerComponent = GetControllerFromActorInfo()->GetComponentByClass<UPUBGItemManagerComponent>();
	if (ItemManagerComponent == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	for (const FPUBGGiveItemData& GiveItemData : GiveItemDatas)
	{
		UPUBGItemInstance* ItemInstance = NewObject<UPUBGItemInstance>();
		ItemInstance->Init(GiveItemData.GiveItemTemplateClass);
		ItemManagerComponent->TryAddItem(ItemInstance, GiveItemData.GiveCount);
	}
}

void UPUBGGA_GiveItem::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
