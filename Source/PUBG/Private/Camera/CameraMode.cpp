// Kakanu Portfolio Project


#include "Camera/CameraMode.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Characters/PUBGBaseCharacter.h"
UCameraMode::UCameraMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UCameraMode::Init_Implementation(APUBGBaseCharacter* InCharacter)
{
	Character = InCharacter;
	Controller = Cast<AController>(GetOuter());
	SpringArmComponent = Character->FindComponentByClass<USpringArmComponent>();
	CameraComponent = Character->FindComponentByClass<UCameraComponent>();
}

UCurveFloat* UCameraMode::GetCurveFloat()
{
	return CurveFloat;
}

void UCameraMode::OnUpdate_Implementation(float Output)
{
	// 구현
}

void UCameraMode::OnEnd_Implementation()
{
	// 구현
}
