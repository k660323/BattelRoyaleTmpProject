// Kakanu Portfolio Project


#include "Animation/AnimNotify/PUBGAnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

// cpp ��ü�� �ζ������� ��ȯ
#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAnimNotify_SendGameplayEvent)

UPUBGAnimNotify_SendGameplayEvent::UPUBGAnimNotify_SendGameplayEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
	// Notify ��� ȣ�� 
	// �Ϲ����� ��� �ش� Notify�� ť�� �� �ʰ� �����
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
