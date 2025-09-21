// Kakanu Portfolio Project


#include "Animation/AnimInstances/Player/PUBGPlayerAnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerAnimInstance)

void UPUBGPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UPUBGPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// �̵���
	if (bHasAcceleration)
	{
		// ���ĵ� �ִϸ��̼� Ÿ�̸� �ʱ�ȭ
		IdleElpasedTime = 0.0f;
	}
	else
	{
		// ����Ʈ �ִϸ��̼� Ÿ�̸� ����
		IdleElpasedTime += DeltaSeconds;

		// �ش� Ÿ�̸� ������ ���ĵ� �ִϸ��̼� ���� true�� ����
		bShouldEnterRelaxState = (IdleElpasedTime >= EnterRelaxStateThreshold);
	}
}