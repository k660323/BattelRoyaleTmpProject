// Kakanu Portfolio Project


#include "Animation/AnimNotify/PUBGAnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

// cpp 자체를 인라인으로 변환
#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAnimNotify_SendGameplayEvent)

UPUBGAnimNotify_SendGameplayEvent::UPUBGAnimNotify_SendGameplayEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	// Notify 즉시 호출 
	// 일반적인 경우 해당 Notify가 큐에 들어가 늦게 실행됨
	bIsNativeBranchingPoint = true;
}

void UPUBGAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (EventTag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
	}
}
