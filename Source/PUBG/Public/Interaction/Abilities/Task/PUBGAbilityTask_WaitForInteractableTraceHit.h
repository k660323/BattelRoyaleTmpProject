// Kakanu Portfolio Project

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "Interaction/PUBGInteractionQuery.h"
#include "PUBGAbilityTask_WaitForInteractableTraceHit.generated.h"

struct FPUBGInteractionInfo;
class IPUBGInteractable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableChanged, const TArray<FPUBGInteractionInfo>&, InteractableInfos);

UCLASS()
class PUBG_API UPUBGAbilityTask_WaitForInteractableTraceHit : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPUBGAbilityTask_WaitForInteractableTraceHit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta=(HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly = "true"))
	static UPUBGAbilityTask_WaitForInteractableTraceHit* WaitForInteractableTraceHit(UGameplayAbility* OwningAbility, FPUBGInteractionQuery InteractionQuery, ECollisionChannel TraceChannel, FGameplayAbilityTargetingLocationInfo StartLocation, float InteractionTraceRange = 100.0f, float InteractionTraceRate = 0.1f, bool bShowDebug = false);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void PerfromTrace();
	
	void AimWithPlayerControler(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, float MaxRange, FVector& OutTraceEnd, bool bIgnorePitch = false) const;
	bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& OutClippedPosition) const;
	void LineTrace(const FVector& Start, const FVector& End, const FCollisionQueryParams Params, FHitResult& OutHitResult) const;
	
	void UpdateInteractionInfos(const FPUBGInteractionQuery& InInteractionQuery, const TArray<TScriptInterface<IPUBGInteractable>>& Interactables);
	void HighlightInteractables(const TArray<FPUBGInteractionInfo>& InteractionInfos, bool bShouldHighlight);

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractableChanged InteractableChanged;

private:
	UPROPERTY()
	FPUBGInteractionQuery InteractionQuery;

	ECollisionChannel TraceChannel = ECC_Visibility;
	
	UPROPERTY()
	FGameplayAbilityTargetingLocationInfo StartLocation;
	
	float InteractionTraceRange = 100.0f;
	float InteractionTraceRate = 0.1f;
	bool bShowDebug = false;

	FTimerHandle TraceTimerHandle;
	// IPUBGInteractable 인터페이스가 반환하는 FPUBGInteractionInfo를 캐싱하는 배열
	TArray<FPUBGInteractionInfo> CurrentInteractionInfos;
};
