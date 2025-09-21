// Kakanu Portfolio Project

#pragma once

#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PUBGAnimNotify_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Send Gameplay Event"))
class PUBG_API UPUBGAnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPUBGAnimNotify_SendGameplayEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	FGameplayEventData EventData;
};
