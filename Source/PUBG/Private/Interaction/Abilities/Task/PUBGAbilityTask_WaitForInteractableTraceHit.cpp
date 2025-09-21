
#include "Interaction/Abilities/Task/PUBGAbilityTask_WaitForInteractableTraceHit.h"
#include "Interaction/PUBGInteractable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilityTask_WaitForInteractableTraceHit)

UPUBGAbilityTask_WaitForInteractableTraceHit::UPUBGAbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UPUBGAbilityTask_WaitForInteractableTraceHit* UPUBGAbilityTask_WaitForInteractableTraceHit::WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FPUBGInteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionTraceRange, float InteractionTraceRate, bool bShowDebug)
{
	UPUBGAbilityTask_WaitForInteractableTraceHit* WaitForInteractableTraceHit = NewAbilityTask<UPUBGAbilityTask_WaitForInteractableTraceHit>(OwningAbility);
	WaitForInteractableTraceHit->InteractionQuery = InteractionQuery;
	WaitForInteractableTraceHit->TraceChannel = TraceChannel;
	WaitForInteractableTraceHit->StartLocation = StartLocation;
	WaitForInteractableTraceHit->InteractionTraceRange = InteractionTraceRange;
	WaitForInteractableTraceHit->InteractionTraceRate = InteractionTraceRate;
	WaitForInteractableTraceHit->bShowDebug = bShowDebug;
	
	return WaitForInteractableTraceHit;
}

void UPUBGAbilityTask_WaitForInteractableTraceHit::Activate()
{
	Super::Activate();

	SetWaitingOnAvatar();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::PerfromTrace, InteractionTraceRate, true);
	}

}

void UPUBGAbilityTask_WaitForInteractableTraceHit::OnDestroy(bool bInOwnerFinished)
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

void UPUBGAbilityTask_WaitForInteractableTraceHit::PerfromTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	if (AvatarActor == nullptr)
	{
		return;
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AvatarActor);
	// ���Ϳ� ������ ���͵��� �ش� �迭�� �־ ��ȯ�Ѵ�.
	AvatarActor->GetAttachedActors(ActorsToIgnore, false, true);

	// �浹 ���� �Ķ����
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PUBGAbilityTask_WaitForInteractableTraceHit), false);
	Params.AddIgnoredActors(ActorsToIgnore);

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	AimWithPlayerControler(AvatarActor, Params, TraceStart, InteractionTraceRange, TraceEnd);

	FHitResult HitResult;
	LineTrace(TraceStart, TraceEnd, Params, HitResult);

	TArray<TScriptInterface<IPUBGInteractable>> Interactables;
	TScriptInterface<IPUBGInteractable> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		Interactables.AddUnique(InteractableActor);
	}

	TScriptInterface<IPUBGInteractable> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		Interactables.AddUnique(InteractableComponent);
	}

	UpdateInteractionInfos(InteractionQuery, Interactables);

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		FColor DebugColor = HitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, DebugColor, false, InteractionTraceRate);
			DrawDebugSphere(GetWorld(), HitResult.Location, 5.f, 15, DebugColor, false, InteractionTraceRate);
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, InteractionTraceRate);
		}
	}
#endif // ENABLE_DRAW_DEBUG

}

void UPUBGAbilityTask_WaitForInteractableTraceHit::AimWithPlayerControler(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch) const
{
	if (Ability == nullptr)
	{
		return;
	}

	APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();

	if (PlayerController == nullptr)
	{
		return;
	}

	FVector CameraStart;
	FRotator CameraRotation;
	// �÷��̾��� ī�޶��� ��ġ�� ȸ������ �����´�.
	PlayerController->GetPlayerViewPoint(CameraStart, CameraRotation);

	// ī�޶� ����� ī�޶� Ray�� ���� ���Ѵ�.
	const FVector CameraDirection = CameraRotation.Vector();
	FVector CameraEnd = CameraStart + (CameraDirection * MaxRange);

	// ī�޶� ������ Ray�� �÷��̾� ��ġ ������ ���ͷ��� ���� ����(Sphere) �̳��� �����Ѵ�
	ClipCameraRayToAbilityRange(CameraStart, CameraDirection, TraceStart, MaxRange, CameraEnd);

	FHitResult HitResult;
	LineTrace(CameraStart, CameraEnd, Params, HitResult);

	// 1. Hit�� ��ü�� ���ͷ��� ���� ����(Sphere) �̳����, Hit ��ġ�� TraceEnd ��ġ�� ���Ѵ�.
	// 2. Hit�� ��ü�� ���ų� Hit�� ��ü�� ���ͷ��� ���� ����(Sphere)�� ���� ���ٸ�, Hit ��ġ�� �����ϰ� CameraEnd�� TraceEnd�� ���Ѵ�.
	// - ���Ŀ� �÷��̾�� CameraEnd ������ ��ü�� üũ�Ѵ�.
	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : CameraEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = CameraDirection;
	}

	// TraceEnd�� �ִ� ���ͷ��� ���� ��ġ(Sphere�� ǥ��)���� Ȯ���Ѵ�.
	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UPUBGAbilityTask_WaitForInteractableTraceHit::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const
{
	// �÷��̾� ��ġ���� ī�޶� ��ġ�� ���� ����ȭ�������� ���� ���͸� ���Ѵ�.
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	// ������ �Ѵ�.(ī�޶� ������ ī�޶�->������ ���ͷ� ������������ ���̸� ��ȯ)
	float DistanceCameraToDot = FVector::DotProduct(CameraToCenter, CameraDirection);
	// �������� ����� ���� ������ �ٶ�� �ִ�. (���� ����� ������ ���� ���� ����)
	if (DistanceCameraToDot > 0)
	{
		// ī�޶�->������ ������ ��������  - ������ ���������� �� �Ÿ� ������ ���Ѵ�.
		// �� ������ ���̸� ���Ѵ�. (CameraToCenter ���� DistanceCameraToDot�� ���ų� �۴�)
		float DistanceSquared = CameraToCenter.SizeSquared() - (DistanceCameraToDot * DistanceCameraToDot);
		// �� ũ��
		float RadiusSquared = (AbilityRange * AbilityRange);
		// �� ���� ���̰� �� ũ�⺸�� ���ų� ������ȴ�.
		if (DistanceSquared <= RadiusSquared)
		{
			// ���̸� ��ũ�⸸ŭ ����
			float DistanceDotToSphere = FMath::Sqrt(RadiusSquared - DistanceSquared);
			float DistanceCameraToSphere = DistanceCameraToDot + DistanceDotToSphere;
			OutClippedPosition = CameraLocation + (DistanceCameraToSphere * CameraDirection);
			return true;
		}
	}

	return false;
}

void UPUBGAbilityTask_WaitForInteractableTraceHit::LineTrace(const FVector& Start, const FVector& End, const FCollisionQueryParams Params, FHitResult& OutHitResult) const
{
	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, TraceChannel, Params);

	if (HitResults.Num() > 0)
	{
		OutHitResult = HitResults[0];
	}
	else
	{
		OutHitResult = FHitResult();
		OutHitResult.TraceStart = Start;
		OutHitResult.TraceEnd = End;
	}
}

void UPUBGAbilityTask_WaitForInteractableTraceHit::UpdateInteractionInfos(const FPUBGInteractionQuery& InInteractionQuery, const TArray<TScriptInterface<IPUBGInteractable>>& Interactables)
{
	TArray<FPUBGInteractionInfo> InteractionInfos;

	for (const TScriptInterface<IPUBGInteractable>& Interactable : Interactables)
	{
		TArray<FPUBGInteractionInfo> TempInteractionInfos;
		FPUBGInteractionInfoBuilder InteractionInfoBuilder(Interactable, TempInteractionInfos);
		Interactable->GatherPostInteractionInfos(InInteractionQuery, InteractionInfoBuilder);

		for (FPUBGInteractionInfo& InteractionInfo : TempInteractionInfos)
		{
			if (Interactable->CanInteraction(InInteractionQuery))
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
		// ���̶���Ʈ
		HighlightInteractables(CurrentInteractionInfos, false);
		CurrentInteractionInfos = InteractionInfos;
		HighlightInteractables(CurrentInteractionInfos, true);

		// �ݹ��Լ� ȣ��
		InteractableChanged.Broadcast(CurrentInteractionInfos);
	}
}

void UPUBGAbilityTask_WaitForInteractableTraceHit::HighlightInteractables(const TArray<FPUBGInteractionInfo>& InteractionInfos, bool bShouldHighlight)
{
	TArray<UMeshComponent*> MeshComponents;

	for (const FPUBGInteractionInfo& InteractionInfo : CurrentInteractionInfos)
	{
		if (IPUBGInteractable* Interactable = InteractionInfo.Interactable.GetInterface())
		{
			Interactable->GetMeshComponents(MeshComponents);
		}
	}

	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetRenderCustomDepth(bShouldHighlight);
	}
}
