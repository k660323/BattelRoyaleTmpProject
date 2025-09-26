// Kakanu Portfolio Project

#pragma once

#include "Components/ControllerComponent.h"
#include "CameraSpringArmTimelineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndDelegate);

class UCameraMode;
class UTimelineComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UCameraSpringArmTimelineComponent : public UControllerComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraSpringArmTimelineComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:	
	bool IsPlayableTimeline(int32 InPrioirty);
	void PlayTimeline(TSubclassOf<UCameraMode> CameraModeClass, int32 InPrioirty, float InPlayRate = 1.0f, bool InbReverse = false);
	void StopTimeline();

protected:
	UFUNCTION()
	void OnEnd();

public:
	FEndDelegate EndDelegate;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> TimelineComponent;

	UPROPERTY()
	TObjectPtr<UCameraMode> CameraMode;

	UPROPERTY()
	int32 Prioirty;


};
