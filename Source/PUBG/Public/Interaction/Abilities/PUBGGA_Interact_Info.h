// Kakanu Portfolio Project

#pragma once

#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "Interaction/PUBGInteractable.h"
#include "PUBGGA_Interact_Info.generated.h"


UCLASS(Abstract)
class PUBG_API UPUBGGA_Interact_Info : public UPUBGGameplayAbility
{
	GENERATED_BODY()

public:
	UPUBGGA_Interact_Info(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UFUNCTION(BlueprintCallable)
	bool InitInteractionData(AActor* TargetActor);

protected:
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IPUBGInteractable> Interactable;

	UPROPERTY(BlueprintReadOnly)
	FPUBGInteractionInfo InteractionInfo;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> InteractableActor;
};