// Kakanu Portfolio Project

#include "Interaction/Abilities/Task/PUBGAbilityTask_WaitForInteractableSphereTrace.h"
#include "Interaction/PUBGInteractable.h"
#include "Engine/OverlapResult.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilityTask_WaitForInteractableSphereTrace)

UPUBGAbilityTask_WaitForInteractableSphereTrace::UPUBGAbilityTask_WaitForInteractableSphereTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UPUBGAbilityTask_WaitForInteractableSphereTrace* UPUBGAbilityTask_WaitForInteractableSphereTrace::WaitForInteractableSphereTrace(UGameplayAbility* OwningAbility, FPUBGInteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionRadius, float InteractionRate, bool bShowDebug)
{
	UPUBGAbilityTask_WaitForInteractableSphereTrace* SphereTraceTask = NewAbilityTask<UPUBGAbilityTask_WaitForInteractableSphereTrace>(OwningAbility);
	SphereTraceTask->InteractionQuery = InteractionQuery;
	SphereTraceTask->TraceChannel = TraceChannel;
	SphereTraceTask->StartLocation = StartLocation;
	SphereTraceTask->InteractionRadius = InteractionRadius;
	SphereTraceTask->InteractionRate = InteractionRate;
	SphereTraceTask->bShowDebug = bShowDebug;

	return SphereTraceTask;
}

void UPUBGAbilityTask_WaitForInteractableSphereTrace::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerformTrace, InteractionRate, true);
	}
}

void UPUBGAbilityTask_WaitForInteractableSphereTrace::OnDestroy(bool bInOwnerFinished)
{
	if (TraceTimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(TraceTimerHandle);
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UPUBGAbilityTask_WaitForInteractableSphereTrace::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (AvatarActor == nullptr)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FVector TraecStart = StartLocation.GetTargetingTransform().GetLocation();

	// 충돌 쿼리 파라미터
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(AvatarActor);
	AvatarActor->GetAttachedActors(IgnoreActors, false, true);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(PUBGAbilityTask_WaitForInteractableTraceHit), false);
	Params.AddIgnoredActors(IgnoreActors);

	

	GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		TraecStart,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(InteractionRadius),
		Params
	);

	TArray<TScriptInterface<IPUBGInteractable>> Interactables;

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		TScriptInterface<IPUBGInteractable> InteractableActor(OverlapResult.GetActor());

		if (InteractableActor)
		{
			Interactables.AddUnique(InteractableActor);
		}

		TScriptInterface<IPUBGInteractable> InteractableComponent(OverlapResult.GetComponent());

		if (InteractableComponent)
		{
			Interactables.AddUnique(InteractableComponent);
		}
	}

	UpdateInteractionInfo(Interactables);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugSphere(GetWorld(), StartLocation.GetTargetingTransform().GetLocation(), InteractionRadius, 16, FColor::Yellow, false, InteractionRate);
	}
#endif // ENABLE_DRAW_DEBUG
}

void UPUBGAbilityTask_WaitForInteractableSphereTrace::UpdateInteractionInfo(const TArray<TScriptInterface<IPUBGInteractable>>& Interactables)
{
	TArray<FPUBGInteractionInfo> InteractionInfos;

	for (const TScriptInterface<IPUBGInteractable>& Interactable : Interactables)
	{
		TArray<FPUBGInteractionInfo> TempInteractionInfos;

		FPUBGInteractionInfoBuilder InteractionInfoBuilder(Interactable, TempInteractionInfos);
		Interactable->GatherPostInteractionInfos(InteractionQuery, InteractionInfoBuilder);

		for (const FPUBGInteractionInfo& InteractionInfo : TempInteractionInfos)
		{
			if (Interactable->CanInteraction(InteractionQuery))
			{
				InteractionInfos.Add(InteractionInfo);
			}
		}
	}


	bool bInfosChanged = false;
	if (InteractionInfos.Num() == CurrentInteractionInfos.Num())
	{
		InteractionInfos.Sort();

		for (int32 i = 0; i < InteractionInfos.Num(); i++)
		{
			const FPUBGInteractionInfo& NewInfo = InteractionInfos[i];
			const FPUBGInteractionInfo& CurrentInfo = CurrentInteractionInfos[i];

			if (NewInfo != CurrentInfo)
			{
				bInfosChanged = true;
				break;
			}
		}
	}
	else
	{
		bInfosChanged = true;
	}

	if (bInfosChanged)
	{
		CurrentInteractionInfos = InteractionInfos;
		// 콜백 함수 호출	
		NearInteractableChanged.Broadcast(CurrentInteractionInfos);
	}
}
