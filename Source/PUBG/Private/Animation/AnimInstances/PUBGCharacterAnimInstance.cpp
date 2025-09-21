// Kakanu Portfolio Project


#include "Animation/AnimInstances/PUBGCharacterAnimInstance.h"
#include "Characters/PUBGBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Net/UnrealNetwork.h"

void UPUBGCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

//void UPUBGCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
//{
//	Super::NativeUpdateAnimation(DeltaSeconds);
//
//	if (!OwningCharacter || !OwningMovementComponent)
//	{
//		return;
//	}
//
//	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
//
//	// 벡터를 길이로 변환 후 > 0.f 이면 가속 
//	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
//
//	// 이동 방향과 회전반경의 차이를 반환하는 함수 (-180~180)
//	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
//}

bool UPUBGCharacterAnimInstance::IsPlayingMontageThreadSafe() const
{
	return IsAnyMontagePlaying();
}