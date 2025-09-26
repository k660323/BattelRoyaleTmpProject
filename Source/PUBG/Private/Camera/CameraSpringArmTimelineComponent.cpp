// Kakanu Portfolio Project


#include "Camera/CameraSpringArmTimelineComponent.h"
#include "GameFramework/Controller.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraMode.h"

// Sets default values for this component's properties
UCameraSpringArmTimelineComponent::UCameraSpringArmTimelineComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	CameraMode(nullptr),
	Prioirty(-1)
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UCameraSpringArmTimelineComponent::IsPlayableTimeline(int32 InPrioirty)
{
	// 우선순위가 같거나 높으면 실행 가능
	return InPrioirty >= Prioirty;
}

void UCameraSpringArmTimelineComponent::PlayTimeline(TSubclassOf<UCameraMode> CameraModeClass, int32 InPrioirty, float InPlayRate, bool InbReverse)
{
	if (InPrioirty < Prioirty)
	{
		return;
	}

	if (GetOwner() == nullptr)
	{
		return;
	}

	AController* Controller = Cast<AController>(GetOwner());
	if (Controller == nullptr)
	{
		return;
	}

	APUBGBaseCharacter* Character = Cast<APUBGBaseCharacter>(Controller->GetPawn());
	if (Character == nullptr)
	{
		return;
	}

	StopTimeline();

	TimelineComponent = NewObject<UTimelineComponent>(GetOwner());
	TimelineComponent->RegisterComponent();
	TimelineComponent->SetPlayRate(InPlayRate);

	Prioirty = InPrioirty;

	CameraMode = NewObject<UCameraMode>(GetOwner(), CameraModeClass);
	CameraMode->Init(Character);

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindDynamic(CameraMode, &UCameraMode::OnUpdate);

	UCurveFloat* CurveFloat = CameraMode->GetCurveFloat();
	check(CurveFloat)
	TimelineComponent->AddInterpFloat(CurveFloat, ProgressUpdate);

	FOnTimelineEvent EndEvent;
	EndEvent.BindDynamic(this, &ThisClass::OnEnd);
	TimelineComponent->SetTimelineFinishedFunc(EndEvent);
	
	if (InbReverse)
	{
		TimelineComponent->Reverse();
	}
	else
	{
		TimelineComponent->PlayFromStart();
	}

}

void UCameraSpringArmTimelineComponent::StopTimeline()
{
	if (IsValid(TimelineComponent))
	{
		TimelineComponent->Stop();
		TimelineComponent->UnregisterComponent();
		TimelineComponent->MarkAsGarbage();
		TimelineComponent = nullptr;

		if (EndDelegate.IsBound())
		{
			EndDelegate.Broadcast();
			EndDelegate.Clear();
		}
	}

	if (IsValid(CameraMode))
	{
		CameraMode->MarkAsGarbage();
		CameraMode = nullptr;
	}

	Prioirty = -1;
}

void UCameraSpringArmTimelineComponent::OnEnd()
{
	CameraMode->OnEnd_Implementation();

	if (EndDelegate.IsBound())
	{
		EndDelegate.Broadcast();
		EndDelegate.Clear();
	}

	StopTimeline();
}
