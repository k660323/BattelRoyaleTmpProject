// Kakanu Portfolio Project


#include "Item/Fragments/PUBGItemFragment_Utility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFragment_Utility)

UPUBGItemFragment_Utility::UPUBGItemFragment_Utility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Utility;
}

void UPUBGItemFragment_Utility::OnInstanceCreated(UPUBGItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	AddStatTagToStack(ItemInstance, StatSets);
}
