// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PUBGAnimNotifyState_PerfomTrace.generated.h"

USTRUCT(BlueprintType)
struct FTraceParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TargetDistance = 20.f;

	UPROPERTY(EditAnywhere)
	bool bCharacterMeshSocket = false;

	UPROPERTY(EditAnywhere)
	FName TraceSocketName = "TraceSocket";
};

USTRUCT(BlueprintType)
struct FTraceDebugParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bDrawDebugShape = false;

	UPROPERTY(EditAnywhere)
	FColor TraceColor = FColor::Red;

	UPROPERTY(EditAnywhere)
	FColor HitColor = FColor::Green;
};

class APUBGEquipmentBase;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAnimNotifyState_PerfomTrace : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UPUBGAnimNotifyState_PerfomTrace(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	void PerformTrace(USkeletalMeshComponent* MeshComponent);

	UPROPERTY(EditAnywhere)
	EActiveWeaponEquipmentType ActiveEquipmentState = EActiveWeaponEquipmentType::Unarmed;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ENetRole> ExecuteNetRole = ROLE_Authority;

	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	FTraceParams TraceParams;

	UPROPERTY(EditAnywhere)
	FTraceDebugParams TraceDebugParams;

private:
	UPROPERTY()
	TWeakObjectPtr<APUBGEquipmentBase> WeaponActor;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;

private:
	FTransform PreviousTraceTransform;
	FTransform PreviousDebugTransform;
	FTransform PreviousSocketTransform;
};
