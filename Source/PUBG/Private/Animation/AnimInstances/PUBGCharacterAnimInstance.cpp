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
//	// ���͸� ���̷� ��ȯ �� > 0.f �̸� ���� 
//	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
//
//	// �̵� ����� ȸ���ݰ��� ���̸� ��ȯ�ϴ� �Լ� (-180~180)
//	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
//}

bool UPUBGCharacterAnimInstance::IsPlayingMontageThreadSafe() const
{
	return IsAnyMontagePlaying();
}