// Kakanu Portfolio Project

#pragma once

#include "UObject/NoExportTypes.h"
#include "System/PUBGGameplayTagStack.h"
#include "PUBGItemInstance.generated.h"

class UPUBGItemTemplate;
/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UPUBGItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// UObject 레플리케이션 등록
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

public:
	void Init(int32 InItemTemplateID);
	void Init(TSubclassOf<UPUBGItemTemplate> InItemTemplateClass);

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddStatTagStack(FGameplayTag StatTag, float StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveStatTagStack(FGameplayTag StatTag);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetItemTemplateID() const { return ItemTemplateID; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasStatTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStackCountByTag(FGameplayTag StatTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FPUBGGameplayTagStackContainer& GetStatContainer() const { return StatContainer;  };

public:
	UFUNCTION(BlueprintCallable, BlueprintPure ="false", meta = (DeterminesOutputType = "FragmentClass"))
	const UPUBGItemFragment* FindFragmentByClass(TSubclassOf<UPUBGItemFragment> FragmentClass) const;

	template <typename FragmentClass>
	const FragmentClass* FindFragmentByClass() const
	{
		return (FragmentClass*)FindFragmentByClass(FragmentClass::StaticClass());
	}

private:
//#if UE_WITH_IRIS
//	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
//#endif // UE_WITH_IRIS

private:
	UPROPERTY(Replicated)
	int32 ItemTemplateID = INDEX_NONE;

	UPROPERTY(Replicated)
	FPUBGGameplayTagStackContainer StatContainer;
};
