// Kakanu Portfolio Project

#include "Interaction/Abilities/PUBGGA_Interact_Info.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGA_Interact_Info)

UPUBGGA_Interact_Info::UPUBGGA_Interact_Info(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UPUBGGA_Interact_Info::InitInteractionData(AActor* TargetActor)
{
	// �ش� ���Ͱ� �������̽��� ����� ������ ���
	// ���ͷ��ǿ� �ʿ��� ������ ����
	TScriptInterface<IPUBGInteractable> TargetInteractable(TargetActor);
	if (TargetInteractable)
	{
		FPUBGInteractionQuery InteractionQuery;
		InteractionQuery.RequestingAvatar = GetAvatarActorFromActorInfo();
		InteractionQuery.RequestingController = GetControllerFromActorInfo();

		Interactable = TargetInteractable;
		InteractableActor = TargetActor;

		TArray<FPUBGInteractionInfo> InteractionInfos;
		FPUBGInteractionInfoBuilder InteractionInfoBuilder(Interactable, InteractionInfos);

		TargetInteractable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);
		InteractionInfo = InteractionInfos[0];

		return true;
	}

	return false;
}
