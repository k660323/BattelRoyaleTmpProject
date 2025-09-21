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

	// 이동시
	if (bHasAcceleration)
	{
		// 스탠드 애니메이션 타이머 초기화
		IdleElpasedTime = 0.0f;
	}
	else
	{
		// 스탠트 애니메이션 타이머 증가
		IdleElpasedTime += DeltaSeconds;

		// 해당 타이머 만족시 스탠드 애니메이션 변수 true로 설정
		bShouldEnterRelaxState = (IdleElpasedTime >= EnterRelaxStateThreshold);
	}
}