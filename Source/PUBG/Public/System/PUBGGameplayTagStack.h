// Kakanu Portfolio Project

#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PUBGGameplayTagStack.generated.h"

struct FPUBGGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

USTRUCT(BlueprintType)
struct FPUBGGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	FPUBGGameplayTagStack() {}
	FPUBGGameplayTagStack(FGameplayTag InTag, float InStatCount)
		: Tag(InTag), StackCount(InStatCount) { }

public:
	const FGameplayTag& GetStackTag() const { return Tag; }
	const float GetStackCount() const { return StackCount; }

	FString GetDebugString() const;

private:
	friend FPUBGGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	float StackCount = 0.0f;
};


USTRUCT(BlueprintType)
struct FPUBGGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	FPUBGGameplayTagStackContainer() {}

public:
	void AddStack(FGameplayTag Tag, float StackCount);
	void RemoveStack(FGameplayTag Tag);

public:
	const TArray<FPUBGGameplayTagStack>& GetStacks() const { return Stacks; }

	float GetStackCount(FGameplayTag Tag) const { return TagToCountMap.FindRef(Tag); }
	bool ContainsTag(FGameplayTag Tag) const { return TagToCountMap.Contains(Tag); }
	
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	// ��Ŷ ���۽� �ش� �Լ� ȣ��
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPUBGGameplayTagStack, FPUBGGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FPUBGGameplayTagStack> Stacks;

	UPROPERTY(NotReplicated)
	TMap<FGameplayTag, float> TagToCountMap;
};

// ����ȭ ���
template<>
struct TStructOpsTypeTraits<FPUBGGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FPUBGGameplayTagStackContainer>
{
	enum
	{
		// ����ü �迭 ����ȭ ���
		WithNetDeltaSerializer = true,
	};
};