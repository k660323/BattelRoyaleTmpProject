
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
	// 액터에 부착된 액터들을 해당 배열에 넣어서 반환한다.
	AvatarActor->GetAttachedActors(ActorsToIgnore, false, true);

	// 충돌 쿼리 파라미터
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
	// 플레이어의 카메라의 위치와 회전값을 가져온다.
	PlayerController->GetPlayerViewPoint(CameraStart, CameraRotation);

	// 카메라 방향과 카메라 Ray의 끝을 구한다.
	const FVector CameraDirection = CameraRotation.Vector();
	FVector CameraEnd = CameraStart + (CameraDirection * MaxRange);

	// 카메라 방향의 Ray를 플레이어 위치 기준의 인터렉션 가능 범위(Sphere) 이내로 제한한다
	ClipCameraRayToAbilityRange(CameraStart, CameraDirection, TraceStart, MaxRange, CameraEnd);

	FHitResult HitResult;
	LineTrace(CameraStart, CameraEnd, Params, HitResult);

	// 1. Hit된 물체가 인터렉션 가능 범위(Sphere) 이내라면, Hit 위치를 TraceEnd 위치로 정한다.
	// 2. Hit된 물체가 없거나 Hit된 물체가 인터렉션 가능 범위(Sphere)를 벗어 났다면, Hit 위치를 무시하고 CameraEnd를 TraceEnd로 정한다.
	// - 이후에 플레이어와 CameraEnd 사이의 물체를 체크한다.
	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));
	const FVector AdjustedEnd = bUseTraceResult ? HitResult.Location : CameraEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = CameraDirection;
	}

	// TraceEnd를 최대 인터렉션 가능 위치(Sphere의 표면)까지 확장한다.
	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UPUBGAbilityTask_WaitForInteractableTraceHit::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const
{
	// 플레이어 위치에서 카메라 위치를 빼서 정규화되지않은 방향 벡터를 구한다.
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	// 내적을 한다.(카메랑 방향을 카메라->센터의 벡터로 투명했을때의 길이를 반환)
	float DistanceCameraToDot = FVector::DotProduct(CameraToCenter, CameraDirection);
	// 내적값이 양수면 같은 방향을 바라고 있다. (게임 설계상 음수가 나올 경우는 없음)
	if (DistanceCameraToDot > 0)
	{
		// 카메라->센터의 벡터의 길이제곱  - 내적의 길이제곱을 뺀 거리 제곱을 구한다.
		// 두 길이의 차이를 구한다. (CameraToCenter 보다 DistanceCameraToDot는 같거나 작다)
		float DistanceSquared = CameraToCenter.SizeSquared() - (DistanceCameraToDot * DistanceCameraToDot);
		// 구 크기
		float RadiusSquared = (AbilityRange * AbilityRange);
		// 두 길이 차이가 구 크기보다 같거나 작으면된다.
		if (DistanceSquared <= RadiusSquared)
		{
			// 길이를 구크기만큼 보정
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
		// 하이라이트
		HighlightInteractables(CurrentInteractionInfos, false);
		CurrentInteractionInfos = InteractionInfos;
		HighlightInteractables(CurrentInteractionInfos, true);

		// 콜백함수 호출
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
