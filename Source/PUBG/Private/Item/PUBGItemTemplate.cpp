// Kakanu Portfolio Project


#include "Item/PUBGItemTemplate.h"
#include "Item/PUBGItemInstance.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemTemplate)

void UPUBGItemFragment::AddStatTagToStack(UPUBGItemInstance* ItemInstance, const TArray<FStatSet>& StatSets) const
{
	if (ItemInstance == nullptr)
	{
		return;
	}

	for (const FStatSet& StatSet : StatSets)
	{
		const FGameplayTag& StatTag = StatSet.StatTag;
		const float& Value = StatSet.Value;
		ItemInstance->AddStatTagStack(StatTag, Value);
	}
}


void UPUBGItemFragment::AddStatTagToStack(UPUBGItemInstance* ItemInstance, const TArray<FInitStatSet>& StatSets) const
{
	if (ItemInstance == nullptr)
	{
		return;
	}

	for (const FInitStatSet& StatSet : StatSets)
	{
		const FGameplayTag& MaxStatTag = StatSet.MaxStatTag;
		const FGameplayTag& CurStatTag = StatSet.CurStatTag;
		const float& Value = StatSet.Value;
		ItemInstance->AddStatTagStack(MaxStatTag, Value);
		ItemInstance->AddStatTagStack(CurStatTag, Value);
	}
}

UPUBGItemTemplate::UPUBGItemTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

#if WITH_EDITOR
EDataValidationResult UPUBGItemTemplate::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = UObject::IsDataValid(Context);

	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("MaxStackCount is less than 1"))));
		Result = EDataValidationResult::Invalid;
	}

	if (Weight < 0)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("Weight is less then 0"))));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif // WITH_EDITOR

const UPUBGItemFragment* UPUBGItemTemplate::FindFragmentByClass(TSubclassOf<UPUBGItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (const UPUBGItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}