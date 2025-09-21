// Kakanu Portfolio Project


#include "Item/Fragments/PUBGItemFragment_GunAcc.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFragment_GunAcc)

UPUBGItemFragment_GunAcc::UPUBGItemFragment_GunAcc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EquipmentType = EEquipmentType::GunAccessory;


}

void UPUBGItemFragment_GunAcc::OnInstanceCreated(UPUBGItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);


}

bool UPUBGItemFragment_GunAcc::IsEquipableWeaponItem(TSubclassOf<UPUBGItemTemplate> WeaponItemTemplate) const
{
	return GunAccMeshMap.Contains(WeaponItemTemplate);
}
