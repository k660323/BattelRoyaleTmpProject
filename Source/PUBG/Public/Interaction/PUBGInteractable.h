// Kakanu Portfolio Project

#pragma once

#include "UObject/Interface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Attributes/PUBGCombatSet.h"
#include "PUBGInteractionInfo.h"
#include "PUBGInteractionQuery.h"
#include "PUBGInteractable.generated.h"

class IPUBGInteractable;
struct FPUBGInteractionInfo;

class FPUBGInteractionInfoBuilder
{
public:
	FPUBGInteractionInfoBuilder(TScriptInterface<IPUBGInteractable> InInteractable, TArray<FPUBGInteractionInfo>& InInteractionInfos)
		: Interactable(InInteractable),
		InteractionInfos(InInteractionInfos) { }

public:
	// 배열에 값을 넣고 데이터에서 다시 인터페이스 역으로 참조가능하게 설정
	void AddInteractionInfo(const FPUBGInteractionInfo& InteractionInfo)
	{
		FPUBGInteractionInfo& Entry = InteractionInfos.Add_GetRef(InteractionInfo);
		Entry.Interactable = Interactable;
	}

private:
	TScriptInterface<IPUBGInteractable> Interactable;
	TArray<FPUBGInteractionInfo>& InteractionInfos;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPUBGInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PUBG_API IPUBGInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FPUBGInteractionInfo GetPreInteractionInfo(const FPUBGInteractionQuery& InteractionQuery) const { return FPUBGInteractionInfo(); };

	virtual void GatherPostInteractionInfos(const FPUBGInteractionQuery& InteractionQuery, FPUBGInteractionInfoBuilder& InteractionInfoBuilder) const
	{
		FPUBGInteractionInfo InteractionInfo = GetPreInteractionInfo(InteractionQuery);

		if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractionQuery.RequestingAvatar.Get()))
		{
			float Resourcefulness = AbilitySystem->GetNumericAttribute(UPUBGCombatSet::GetResourcefulnessAttribute());
			InteractionInfo.Duration = FMath::Max<float>(0.f, InteractionInfo.Duration - Resourcefulness * 0.01f);
		}

		InteractionInfoBuilder.AddInteractionInfo(InteractionInfo);
	}

	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) const {}
	
	UFUNCTION(BlueprintCallable)
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const {}

	UFUNCTION(BlueprintCallable)
	virtual bool CanInteraction(const FPUBGInteractionQuery& InteractionQuery) const { return true;  }
};
