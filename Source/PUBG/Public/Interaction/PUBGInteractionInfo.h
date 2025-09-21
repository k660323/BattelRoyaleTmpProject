// Kakanu Portfolio Project

#pragma once

#include "Abilities/GameplayAbility.h"
#include "PUBGInteractionInfo.generated.h"

class IPUBGInteractable;

USTRUCT(BlueprintType)
struct FPUBGInteractionInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IPUBGInteractable> Interactable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 0.f;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityToGrant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> ActiveStartMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> ActiveEndMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;

public:
	FORCEINLINE bool operator==(const FPUBGInteractionInfo& Other) const
	{
		return Interactable == Other.Interactable &&
			Title.IdenticalTo(Other.Title) &&
			Content.IdenticalTo(Other.Content) &&
			Duration == Other.Duration &&
			AbilityToGrant == Other.AbilityToGrant &&
			ActiveStartMontage == Other.ActiveStartMontage &&
			ActiveEndMontage == Other.ActiveEndMontage &&
			InteractionWidgetClass == Other.InteractionWidgetClass;
	}

	FORCEINLINE bool operator !=(const FPUBGInteractionInfo& Other) const
	{
		return !operator==(Other);
	}

	FORCEINLINE bool operator<(const FPUBGInteractionInfo& Other) const
	{
		return Interactable.GetInterface() < Other.Interactable.GetInterface();
	}

};