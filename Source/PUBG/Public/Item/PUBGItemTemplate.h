// Kakanu Portfolio Project

#pragma once

#include "UObject/NoExportTypes.h"
#include "PUBGItemTemplate.generated.h"

class UPUBGItemInstance;

USTRUCT(BlueprintType)
struct FStatSet
{
	GENERATED_BODY()

public:
	FStatSet() : StatTag(), Value(0.0f) {}

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "SetByCaller,Variable.Item"))
	FGameplayTag StatTag;

	UPROPERTY(EditDefaultsOnly)
	float Value;
};

USTRUCT(BlueprintType)
struct FInitStatSet
{
	GENERATED_BODY()

public:
	FInitStatSet() : MaxStatTag(), CurStatTag(), Value(0.0f) {}

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "SetByCaller,Variable.Item"))
	FGameplayTag MaxStatTag;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "SetByCaller,Variable.Item"))
	FGameplayTag CurStatTag;

	UPROPERTY(EditDefaultsOnly)
	float Value;
};

// DefaultToInstanced : Ŭ������ ��������� ������ �ڵ����� �ν��Ͻ� ����
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UPUBGItemFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(UPUBGItemInstance* Instance) const {}
	virtual bool GetDefaultStackValueByTag(FGameplayTag& InTag, float& Value) const { return false; };

protected:
	void AddStatTagToStack(UPUBGItemInstance* ItemInstance, const TArray<FStatSet>& StatSets) const;
	void AddStatTagToStack(UPUBGItemInstance* ItemInstance, const TArray<FInitStatSet>& StatSets) const;
};

/**
 * 
 */
UCLASS(Blueprintable, Const, Abstract)
class PUBG_API UPUBGItemTemplate : public UObject
{
	GENERATED_BODY()
	
public:
	UPUBGItemTemplate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
#if WITH_EDITOR
	// �����ܿ��� ������ ��ȿ�� �˻�
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

public:
	// PUBGItemFragment Ŭ������ ������ �Լ�
	UFUNCTION(BlueprintCallable, BlueprintPure="false", meta = (DeterminesOutputType="FragmentClass"))
	const UPUBGItemFragment* FindFragmentByClass(TSubclassOf<UPUBGItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStackCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> PickupableMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform PickupableMeshOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform PickupableCollisionBoxOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector PickupableBoxExtentOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight;

	// Instanced : ���� �ν��Ͻ� ���
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UPUBGItemFragment>> Fragments;
};
