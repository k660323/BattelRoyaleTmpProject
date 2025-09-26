// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/PUBGBaseCharacter.h"
#include "PUBGPlayerCharacter.generated.h"

class UPUBGAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class PUBG_API APUBGPlayerCharacter : public APUBGBaseCharacter
{
	GENERATED_BODY()
	
public:
	APUBGPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void InitializeAbilitySystem() override;
	virtual void UninitializeAbilitySystem() override;

	UFUNCTION(Server, Reliable)
	void Server_InitializeDefaultAbility();

	UFUNCTION(Server, Reliable)
	void Server_UnInitializeDefaultAbility();


	UFUNCTION(BlueprintCallable)
	void SetCameraFreeViewMode(bool IsOn);

	UFUNCTION(Server, Reliable)
	void Server_SetCameraFreeViewMode(bool IsOn);

	void SetProneSpringArm(bool IsOn);

	void SetFPSMode(bool IsOn);

	bool IsFPSMode();

public:
	FORCEINLINE USpringArmComponent* GetFPSSpringArmComponent() const
	{
		return FPSSpringArmComponent;
	}

	FORCEINLINE UCameraComponent* GetFPSCameraComponent() const
	{
		return FPSCamera;
	}

	FORCEINLINE USkeletalMeshComponent* GetFPSArmComponent() const
	{
		return FPSArm;
	}

protected:
	UPROPERTY(EditDefaultsOnly)
	USpringArmComponent* FPSSpringArmComponent;

	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* FPSCamera;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* FPSArm;

	UPROPERTY()
	bool bIsFPSMode;

protected:
	virtual void OnRep_Owner() override;
	virtual void OnRep_PlayerState() override;
};
