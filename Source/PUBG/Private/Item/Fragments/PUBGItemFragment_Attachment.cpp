// Kakanu Portfolio Project


#include "Item/Fragments/PUBGItemFragment_Attachment.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFragment_Attachment)

UPUBGItemFragment_Attachment::UPUBGItemFragment_Attachment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AttachInfo.AttachSocketMap.Add({ PUBGGameplayTags::Status_Stand, NAME_None });
	AttachInfo.AttachSocketMap.Add({ PUBGGameplayTags::Status_Crouch, NAME_None });
	AttachInfo.AttachSocketMap.Add({ PUBGGameplayTags::Status_Prone, NAME_None });
	AttachInfo.AttachSocketMap.Add({ PUBGGameplayTags::Status_Aiming, NAME_None });


	EquipMontageMap.Add({ PUBGGameplayTags::Status_Stand, nullptr });
	EquipMontageMap.Add({ PUBGGameplayTags::Status_Crouch, nullptr });
	EquipMontageMap.Add({ PUBGGameplayTags::Status_Prone, nullptr });
	
	UnequipMontageMap.Add({ PUBGGameplayTags::Status_Stand, nullptr });
	UnequipMontageMap.Add({ PUBGGameplayTags::Status_Crouch, nullptr });
	UnequipMontageMap.Add({ PUBGGameplayTags::Status_Prone, nullptr });
}

FName UPUBGItemFragment_Attachment::GetActiveEquipmentAttachSocket(UPUBGAbilitySystemComponent* ASC) const
{
	FName AttachSocket = NAME_None;

	if (ASC == nullptr)
	{
		return AttachSocket;
	}

	if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Aiming))
	{
		AttachSocket = AttachInfo.AttachSocketMap[PUBGGameplayTags::Status_Aiming];
	}
	else if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Crouch))
	{
		AttachSocket = AttachInfo.AttachSocketMap[PUBGGameplayTags::Status_Crouch];
	}
	else if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Prone))
	{
		AttachSocket = AttachInfo.AttachSocketMap[PUBGGameplayTags::Status_Prone];
	}
	else
	{
		AttachSocket = AttachInfo.AttachSocketMap[PUBGGameplayTags::Status_Stand];
	}

	return AttachSocket;
}

TSoftObjectPtr<UAnimMontage> UPUBGItemFragment_Attachment::GetActiveEquipmentMontageByStateTag(UPUBGAbilitySystemComponent* ASC) const
{
	if (ASC == nullptr)
	{
		return nullptr;
	}

	if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Crouch))
	{
		return EquipMontageMap[PUBGGameplayTags::Status_Crouch];
	}
	else if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Prone))
	{
		return EquipMontageMap[PUBGGameplayTags::Status_Prone];
	}
	else
	{
		return EquipMontageMap[PUBGGameplayTags::Status_Stand];
	}
}

TSoftObjectPtr<UAnimMontage> UPUBGItemFragment_Attachment::GetUnActiveEquipmentMontageByStateTag(UPUBGAbilitySystemComponent* ASC) const
{
	if (ASC == nullptr)
	{
		return nullptr;
	}

	if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Crouch))
	{
		return UnequipMontageMap[PUBGGameplayTags::Status_Crouch];
	}
	else if (ASC->HasMatchingGameplayTag(PUBGGameplayTags::Status_Prone))
	{
		return UnequipMontageMap[PUBGGameplayTags::Status_Prone];
	}
	else
	{
		return UnequipMontageMap[PUBGGameplayTags::Status_Stand];
	}
}
