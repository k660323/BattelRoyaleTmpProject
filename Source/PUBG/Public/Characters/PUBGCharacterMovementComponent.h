// Kakanu Portfolio Project

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"
#include "PUBGCharacterMovementComponent.generated.h"

struct FFrame;

class UObject;

PUBG_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);
PUBG_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MoveStopped);
PUBG_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_RotationStopped);


/**
 * FPUBGCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FPUBGCharacterGroundInfo
{
	GENERATED_BODY()

	FPUBGCharacterGroundInfo() 
		: LastUpdateFrame(0)
		, GroundDistance(0)
	{

	}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

/**
 * UPUBGCharacterMovementComponent
 *
 *	The base character movement component class used by this project.
 */
UCLASS(Config = Game)
class PUBG_API UPUBGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UPUBGCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 움직임 예측
	virtual void SimulateMovement(float DeltaTime) override;

	// 점프 시도
	virtual bool CanAttemptJump() const override;

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "PUBG|CharacterMovement")
	const FPUBGCharacterGroundInfo& GetGroundInfo();

	void SetReplicatedAcceleration(const FVector& InAcceleration);

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

protected:
	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FPUBGCharacterGroundInfo CachedGroundInfo;

	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;

protected:
	UPROPERTY(EditDefaultsOnly)
	float LeftRightMovePercent = 0.666f;

	UPROPERTY(EditDefaultsOnly)
	float BackwardMovePercent = 0.666f;

	UPROPERTY(EditDefaultsOnly)
	float CrouchMovePercent = 0.666f;

	UPROPERTY(EditDefaultsOnly)
	float ProneMovePercent = 0.2f;

	UPROPERTY(EditDefaultsOnly)
	float AimMovePercent = 0.7f;

	UPROPERTY(EditDefaultsOnly)
	float DrinkMovePercent = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float SprintMovePercent = 1.25f;

	UPROPERTY(EditDefaultsOnly)
	float WalkMovePercent = 0.35f;
};
