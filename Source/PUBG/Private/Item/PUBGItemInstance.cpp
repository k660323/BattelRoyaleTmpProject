// Kakanu Portfolio Project


#include "Item/PUBGItemInstance.h"
//#if UE_WITH_IRIS
//#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
//#endif // UE_WITH_IRIS
#include "Item/PUBGItemTemplate.h"
#include "Data/PUBGItemData.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemInstance)

UPUBGItemInstance::UPUBGItemInstance(const FObjectInitializer& ObjectInitializer)
{
}

void UPUBGItemInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemTemplateID);
	DOREPLIFETIME(ThisClass, StatContainer);
}

void UPUBGItemInstance::Init(int32 InItemTemplateID)
{
	if(InItemTemplateID <= INDEX_NONE)
	{
		return;
	}

	ItemTemplateID = InItemTemplateID;

	const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);
	
	for (const UPUBGItemFragment* Fragment : ItemTemplate.Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UPUBGItemInstance::Init(TSubclassOf<UPUBGItemTemplate> InItemTemplateClass)
{
	if (InItemTemplateClass == nullptr)
	{
		return;
	}

	int32 FindItemTemplateID = UPUBGItemData::Get().FindItemTemplateIDByClass(InItemTemplateClass);
	check((FindItemTemplateID != 0));

	ItemTemplateID = FindItemTemplateID;

	const UPUBGItemTemplate* ItemTemplate = Cast<UPUBGItemTemplate>(InItemTemplateClass->GetDefaultObject());

	for (const UPUBGItemFragment* Fragment : ItemTemplate->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(this);
		}
	}
}

void UPUBGItemInstance::AddStatTagStack(FGameplayTag StatTag, float StackCount)
{
	StatContainer.AddStack(StatTag, StackCount);
}

void UPUBGItemInstance::RemoveStatTagStack(FGameplayTag StatTag)
{
	StatContainer.RemoveStack(StatTag);
}

bool UPUBGItemInstance::HasStatTag(FGameplayTag StatTag) const
{
	return StatContainer.ContainsTag(StatTag);
}

float UPUBGItemInstance::GetStackCountByTag(FGameplayTag StatTag) const
{
	return StatContainer.GetStackCount(StatTag);
}

const UPUBGItemFragment* UPUBGItemInstance::FindFragmentByClass(TSubclassOf<UPUBGItemFragment> FragmentClass) const
{
	if (ItemTemplateID > INDEX_NONE && FragmentClass)
	{
		const UPUBGItemTemplate& ItemTemplate = UPUBGItemData::Get().FindItemTemplateByID(ItemTemplateID);
		return ItemTemplate.FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
//
//#if UE_WITH_IRIS
//void UPUBGItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
//{
//	using namespace UE::Net;
//	FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
//}
//#endif // UE_WITH_IRIS
