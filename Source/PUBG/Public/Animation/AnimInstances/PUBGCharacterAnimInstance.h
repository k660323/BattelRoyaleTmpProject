// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstances/PUBGBaseAnimInstance.h"
#include "PUBGCharacterAnimInstance.generated.h"


class APUBGBaseCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGCharacterAnimInstance : public UPUBGBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	// virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool IsPlayingMontageThreadSafe() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float GroundSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bHasAcceleration;

	// -180 ~ 180 (�̵� ����, ȸ�������� ���̸� ��Ÿ��)
	// 0 (����), 90 (������), -90 (����), 180, -180 (����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float LocomotionDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float Pitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float Yaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float ForwardValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	float SidewaysVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bShouldMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsCrouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsProneing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsUseEquipItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsFreeFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsUmbrellaOpened;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsPlayingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	int32 AimOffsetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintThreadSafe), Category = "AnimData|LocomotionData")
	bool bIsShooting;
};
