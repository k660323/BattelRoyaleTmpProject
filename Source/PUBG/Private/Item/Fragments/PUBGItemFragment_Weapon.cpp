// Kakanu Portfolio Project


#include "Item/Fragments/PUBGItemFragment_Weapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFragment_Weapon)

UPUBGItemFragment_Weapon::UPUBGItemFragment_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::Weapon;

	int32 Count = (int32)EGunAccType::Count;
	for (int32 i = 0; i < Count; i++)
	{
		UseAccMap.Add((EGunAccType)i, false);
	}
}

void UPUBGItemFragment_Weapon::OnInstanceCreated(UPUBGItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
	
	AddStatTagToStack(ItemInstance, StatSets);
}
