// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "PUBGPlayerController.generated.h"

class UPUBGAbilitySystemComponent;
class APUBGPlayerState;
class UCurveFloat;
class UTimelineComponent;
class UPUBGItemManagerComponent;

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class PUBG_API APUBGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APUBGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitInputSystem() override;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Turn(const FInputActionValue& InputActionValue);
	void Input_FreeViewStarted(const FInputActionValue& InputActionValue);
	void Input_FreeViewReleased(const FInputActionValue& InputActionValue);

	void Input_AbilityInputStarted(FGameplayTag InInputTag);
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	void Input_AbilityInputReleased(FGameplayTag InInputTag);

public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused);
	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const;
	APUBGPlayerState* GetPUBGPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "LimitPitch")
	bool LimitPitchAngle(const float Axis);
private:
	UFUNCTION()
	void UpdateReleaseFreeView(float DeltaTime);
	UFUNCTION()
	void EndReleaseFreeView();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemManager")
	TObjectPtr<UPUBGItemManagerComponent> ItemManagerComponent;

private:
	bool IsFreeViewMove;

	FRotator CachedPressedRotator;
	FRotator CachedReleaseRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeViewTimeline", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> FreeViewTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeViewTimeline", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> FreeViewCurve;

	// Yaw 경계값 지정
	float BottomValue;
	float TopValue;

public:
	UFUNCTION(BlueprintCallable)
	void TestEquipAccInventoryFirstItem();
};
