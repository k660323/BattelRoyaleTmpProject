// Kakanu Portfolio Project


#include "System/PUBGGameplayTagStack.h"
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGameplayTagStack)

FString FPUBGGameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%f"), *Tag.ToString(), StackCount);
}

void FPUBGGameplayTagStackContainer::AddStack(FGameplayTag Tag, float StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	for (FPUBGGameplayTagStack& Stack : Stacks)
	{
		if (Stack.Tag == Tag)
		{
			const float NewCount = Stack.StackCount + StackCount;
			Stack.StackCount = NewCount;
			TagToCountMap[Tag] = NewCount;
			MarkItemDirty(Stack);
			return;
		}
	}

	FPUBGGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);
	MarkItemDirty(NewStack);
	TagToCountMap.Add(Tag, StackCount);
}

void FPUBGGameplayTagStackContainer::RemoveStack(FGameplayTag Tag)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	for (auto It = Stacks.CreateIterator(); It; ++It)
	{
		FPUBGGameplayTagStack& Stack = *It;

		if (Stack.Tag == Tag)
		{
			It.RemoveCurrent();
			TagToCountMap.Remove(Tag);
			MarkArrayDirty();
			return;
		}
	}
}

void FPUBGGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToCountMap.Remove(Tag);
	}
}

void FPUBGGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FPUBGGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FPUBGGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FPUBGGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}
