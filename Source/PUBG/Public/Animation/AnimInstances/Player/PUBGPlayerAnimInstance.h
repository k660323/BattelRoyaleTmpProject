// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstances/PUBGCharacterAnimInstance.h"
#include "PUBGPlayerAnimInstance.generated.h"

class APUBGPlayerCharacter;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGPlayerAnimInstance : public UPUBGCharacterAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 타이머가 대기시간이랑 같거나 크면 스탠드 애니메이션 실행
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	// 스탠드 애니메이션 실행 대기 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.f;

	// 스탠드 애니메이션 실행 타이머
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	float IdleElpasedTime;


};
