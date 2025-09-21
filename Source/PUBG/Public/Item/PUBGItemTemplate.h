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

// DefaultToInstanced : 클래스를 멤버변수로 가질시 자동으로 인스턴스 생성
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
	// 엔진단에서 데이터 유효성 검사
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

public:
	// PUBGItemFragment 클래스를 들고오는 함수
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

	// Instanced : 개별 인스턴스 허용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UPUBGItemFragment>> Fragments;
};
