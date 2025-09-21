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
	// Ÿ�̸Ӱ� ���ð��̶� ���ų� ũ�� ���ĵ� �ִϸ��̼� ����
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	// ���ĵ� �ִϸ��̼� ���� ��� �ð�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.f;

	// ���ĵ� �ִϸ��̼� ���� Ÿ�̸�
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintThreadSafe))
	float IdleElpasedTime;


};
