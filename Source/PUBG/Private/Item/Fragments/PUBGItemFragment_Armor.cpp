// Kakanu Portfolio Project


#include "Item/Fragments/PUBGItemFragment_Armor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFragment_Armor)

UPUBGItemFragment_Armor::UPUBGItemFragment_Armor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Armor;
}

void UPUBGItemFragment_Armor::OnInstanceCreated(UPUBGItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	AddStatTagToStack(ItemInstance, StatSets);
}
