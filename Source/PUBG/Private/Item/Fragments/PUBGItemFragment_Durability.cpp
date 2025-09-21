// Kakanu Portfolio Project


#include "Item/Fragments/PUBGItemFragment_Durability.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemFragment_Durability)

UPUBGItemFragment_Durability::UPUBGItemFragment_Durability(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UPUBGItemFragment_Durability::OnInstanceCreated(UPUBGItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);

	AddStatTagToStack(ItemInstance, StatSets);
}

bool UPUBGItemFragment_Durability::GetDefaultStackValueByTag(FGameplayTag& InTag, int32& Value) const
{
	for (FInitStatSet StatSet : StatSets)
	{
		if (StatSet.CurStatTag == InTag || (StatSet.MaxStatTag == InTag))
		{
			Value = StatSet.Value;
			return true;
		}
	}

	return false;
}
