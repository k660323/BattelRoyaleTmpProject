// Kakanu Portfolio Project


#include "Physics/PUBGAnimNotifyState_PerfomTrace.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Item/Managers/PUBGActiveEquipmentManagerComponent.h"
#include "Item/Managers/PUBGEquipmentManagerComponent.h"
#include "Actors/PUBGEquipmentBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAnimNotifyState_PerfomTrace)

UPUBGAnimNotifyState_PerfomTrace::UPUBGAnimNotifyState_PerfomTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UPUBGAnimNotifyState_PerfomTrace::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComponent, Animation, TotalDuration, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
	{
		return;
	}

	if (APUBGBaseCharacter* Character = Cast<APUBGBaseCharacter>(MeshComponent->GetOwner()))
	{
		if (UPUBGEquipmentManagerComponent* EquipmentManager = Character->FindComponentByClass<UPUBGEquipmentManagerComponent>())
		{
			EEquipmentSlotType EquipmentSlot = UPUBGActiveEquipmentManagerComponent::ActiveWeaponEquipmentTypeConvertToEquipmentSlotType(ActiveEquipmentState);
			WeaponActor = EquipmentManager->GetEntryEquipmentActor(EquipmentSlot);
			if (WeaponActor.IsValid() == false)
			{
				return;
			}

			if (TraceParams.bCharacterMeshSocket)
			{
				FTransform SocketTransform = MeshComponent->GetSocketTransform(TraceParams.TraceSocketName);
				PreviousTraceTransform = WeaponActor->GetTPSEquipmentMesh()->GetRelativeTransform() * SocketTransform;
				PreviousDebugTransform = WeaponActor->GetTraceDebugCollision()->GetRelativeTransform() * SocketTransform;
				PreviousSocketTransform = SocketTransform;
			}
			else
			{
				PreviousTraceTransform = WeaponActor->GetTPSEquipmentMesh()->GetComponentTransform();
				PreviousDebugTransform = WeaponActor->GetTraceDebugCollision()->GetComponentTransform();
				PreviousSocketTransform = WeaponActor->GetTPSEquipmentMesh()->GetSocketTransform(TraceParams.TraceSocketName);
			}

			

#if UE_EDITOR
			if (TraceParams.bCharacterMeshSocket)
			{
				check(MeshComponent->DoesSocketExist(TraceParams.TraceSocketName));
			}
			else
			{
				check(WeaponActor->GetTPSEquipmentMesh()->DoesSocketExist(TraceParams.TraceSocketName));
			}
#endif

			HitActors.Empty();
		}
	}
}

void UPUBGAnimNotifyState_PerfomTrace::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComponent, Animation, FrameDeltaTime, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;

	if (WeaponActor.IsValid() == false)
		return;

	PerformTrace(MeshComponent);
}

void UPUBGAnimNotifyState_PerfomTrace::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComponent, Animation, EventReference);

	if (MeshComponent->GetOwnerRole() != ExecuteNetRole)
		return;

	if (WeaponActor.IsValid() == false)
		return;

	PerformTrace(MeshComponent);
}

void UPUBGAnimNotifyState_PerfomTrace::PerformTrace(USkeletalMeshComponent* MeshComponent)
{
	FTransform CurrentSocketTransform = TraceParams.bCharacterMeshSocket ? MeshComponent->GetSocketTransform(TraceParams.TraceSocketName) : WeaponActor->GetTPSEquipmentMesh()->GetSocketTransform(TraceParams.TraceSocketName);
	float Distance = (PreviousSocketTransform.GetLocation() - CurrentSocketTransform.GetLocation()).Length();

	int SubStepCount = FMath::CeilToInt(Distance / TraceParams.TargetDistance);
	if (SubStepCount <= 0)
		return;

	float SubstepRatio = 1.f / SubStepCount;

	FTransform CurrentTraceTransform;
	FTransform CurrentDebugTransform;
	
	if (TraceParams.bCharacterMeshSocket)
	{
		FTransform SocketTransform = MeshComponent->GetSocketTransform(TraceParams.TraceSocketName);
		CurrentTraceTransform = WeaponActor->GetTPSEquipmentMesh()->GetRelativeTransform() * SocketTransform;
		CurrentDebugTransform = WeaponActor->GetTraceDebugCollision()->GetRelativeTransform() * SocketTransform;
	}
	else
	{
		CurrentTraceTransform = WeaponActor->GetTPSEquipmentMesh()->GetComponentTransform();
		CurrentDebugTransform = WeaponActor->GetTraceDebugCollision()->GetComponentTransform();
	}


	TArray<FHitResult> FinalHitResults;

	for (int32 i = 0; i < SubStepCount; i++)
	{
		FTransform StartTraceTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
		FTransform EndTraceTransform = UKismetMathLibrary::TLerp(PreviousTraceTransform, CurrentTraceTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
		FTransform AverageTraceTransform = UKismetMathLibrary::TLerp(StartTraceTransform, EndTraceTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

		FComponentQueryParams Params = FComponentQueryParams::DefaultComponentQueryParams;
		Params.bReturnPhysicalMaterial = true;

		TArray<AActor*> IgnoredActors = { WeaponActor.Get(), WeaponActor->GetOwner() };
		Params.AddIgnoredActors(IgnoredActors);

		TArray<FHitResult> HitResults;
		MeshComponent->GetWorld()->ComponentSweepMulti(HitResults, WeaponActor->GetTPSEquipmentMesh(), StartTraceTransform.GetLocation(), EndTraceTransform.GetLocation(), AverageTraceTransform.GetRotation(), Params);

		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActors.Contains(HitActor) == false)
			{
				HitActors.Add(HitActor);
				FinalHitResults.Add(HitResult);
			}
		}

#if UE_EDITOR
		if (GIsEditor)
		{
			//DeveloperSettings->bForceDisableDebugTrace == false &&
			if (TraceDebugParams.bDrawDebugShape)
			{
				FColor Color = (HitResults.Num() > 0) ? TraceDebugParams.HitColor : TraceDebugParams.TraceColor;

				FTransform StartDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * i, ELerpInterpolationMode::DualQuatInterp);
				FTransform EndDebugTransform = UKismetMathLibrary::TLerp(PreviousDebugTransform, CurrentDebugTransform, SubstepRatio * (i + 1), ELerpInterpolationMode::DualQuatInterp);
				FTransform AverageDebugTransform = UKismetMathLibrary::TLerp(StartDebugTransform, EndDebugTransform, 0.5f, ELerpInterpolationMode::DualQuatInterp);

				DrawDebugSweptBox(MeshComponent->GetWorld(), StartDebugTransform.GetLocation(), EndDebugTransform.GetLocation(), AverageDebugTransform.GetRotation().Rotator(), WeaponActor->GetTraceDebugCollision()->GetScaledBoxExtent(), Color, false, 2.f);
			}
		}
#endif
	}

	PreviousTraceTransform = CurrentTraceTransform;
	PreviousDebugTransform = CurrentDebugTransform;
	PreviousSocketTransform = CurrentSocketTransform;

	if (FinalHitResults.Num() > 0)
	{
		FGameplayAbilityTargetDataHandle TargetDataHandle;

		for (const FHitResult& HitResult : FinalHitResults)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = HitResult;
			TargetDataHandle.Add(NewTargetData);
		}

		FGameplayEventData EventData;
		EventData.TargetData = TargetDataHandle;
		EventData.Instigator = WeaponActor.Get();

		if (EventTag.IsValid())
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComponent->GetOwner(), EventTag, EventData);
		}
	}
}
