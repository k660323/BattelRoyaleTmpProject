// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PUBGDefine.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Data/StartUpData/DataAsset_StartUpDataBase.h"
#include "GameplayEffect.h"
#include "PUBGBaseCharacter.generated.h"


class UPUBGCharacterMovementComponent;
class UPUBGAbilitySystemComponent;
class UPUBGAttributeSet;
class USpringArmComponent;
class UCameraComponent;
class UPUBGActiveEquipmentManagerComponent;
class UPUBGInventoryManagerComponent;
class UPUBGEquipmentManagerComponent;
class APUBGBaseCharacter;
class UPUBGVitalSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPUBGHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPUBGHealth_AttributeChanged, APUBGBaseCharacter*, Character, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * FLyraReplicatedAcceleration: Compressed representation of acceleration
 */
USTRUCT()
struct FPUBGReplicatedAcceleration
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
	uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
	int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};

UCLASS()
class PUBG_API APUBGBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APUBGBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	virtual void InitializeAbilitySystem();
	UFUNCTION()
	virtual void UninitializeAbilitySystem();

	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();

	//~AActor interface
	// virtual void PostActorCreated() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor interface

public:
	// IAbilitySystemInterface 인터페이스
	// 외부에서 어빌리티 컴포넌트 접근 함수
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// -----

	FORCEINLINE UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const
	{
		return PUBGAbilitySystemComponent;
	}

	FORCEINLINE UPUBGAttributeSet* GetPUBGAttributeSetComponent() const
	{
		return PUBGAttributeSet;
	}

	FORCEINLINE UPUBGActiveEquipmentManagerComponent* GetPUBGActiveEquipmentManagerComponent() const
	{
		return PUBGActiveEquipmentManagerComponent;
	}

protected:
	//--------- GAS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	UPUBGAbilitySystemComponent* PUBGAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	UPUBGAttributeSet* PUBGAttributeSet;
	//----------------------

	//--------- 3인칭 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* TPSCameraComponent;
	//----------------------

	// 커스텀 캐릭터 무브먼트
		/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPUBGCharacterMovementComponent> PUBGCharacterMovement;
	//----------------------

	// 매니저 컴포넌트
	// 사용중인 장비
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Manager|ActiveEquipment")
	TObjectPtr<UPUBGActiveEquipmentManagerComponent> PUBGActiveEquipmentManagerComponent;
	
	// 장착중인 장비
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Manager|Equipment")
	TObjectPtr<UPUBGEquipmentManagerComponent> PUBGEquipmentManagerComponent;

	// 인벤토리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Manager|Inventory")
	TObjectPtr<UPUBGInventoryManagerComponent> PUBGInventoryManagerComponent;
	//----------------------

protected:
	UPROPERTY()
	FPUBGStartUpData_GrantedHandles AbilityGrantedHandles;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "AnimData|LocomotionData", meta = (AllowPrivateAccess))
	float AimOffsetControllerYaw;

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FPUBGReplicatedAcceleration ReplicatedAcceleration;

private:
	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

public:
	virtual void RecalculateBaseEyeHeight() override;

public:
	float BaseUnscaledCapsuleHalfHeight = 0.0f;
	
	/** Base eye height above collision center. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float BaseProneEyeHeight = 35.0f;

private:
	bool bInitializedSkeletalMesh = false;

	
protected:
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

public:
	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FPUBGHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FPUBGHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FPUBGHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FPUBGHealth_DeathEvent OnDeathFinished;

protected:
	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UPUBGVitalSet> HealthSet;

public:
	void SetIsDead(bool IsDead);

protected:
	UFUNCTION()
	void OnRep_Dead();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	bool bIsDead = false;

// public:
	///** Set by character movement to specify that this Character is currently crouched. */
	//UPROPERTY(BlueprintReadOnly, replicatedUsing = OnRep_IsProned, Category = Character)
	//uint8 bIsProned : 1;

// public:

	///** Handle Crouching replicated from server */
	//UFUNCTION()
	//virtual void OnRep_IsProned();

	///**
	// * Request the character to start crouching. The request is processed on the next update of the CharacterMovementComponent.
	// * @see OnStartCrouch
	// * @see IsCrouched
	// * @see CharacterMovement->WantsToCrouch
	// */
	//UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	//virtual void Prone(bool bClientSimulation = false);

	///**
	// * Request the character to stop crouching. The request is processed on the next update of the CharacterMovementComponent.
	// * @see OnEndCrouch
	// * @see IsCrouched
	// * @see CharacterMovement->WantsToCrouch
	// */
	//UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	//virtual void UnProne(bool bClientSimulation = false);

	///** @return true if this character is currently able to crouch (and is not currently crouched) */
	//UFUNCTION(BlueprintCallable, Category = Character)
	//virtual bool CanProne() const;

	///**
	// * Called when Character stops crouching. Called on non-owned Characters through bIsCrouched replication.
	// * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	// * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	// */
	//virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	///**
	// * Event when Character stops crouching.
	// * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	// * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	// */
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnEndProne", ScriptName = "OnEndProne"))
	//void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	///**
	// * Called when Character crouches. Called on non-owned Characters through bIsCrouched replication.
	// * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	// * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	// */
	//virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	///**
	// * Event when Character crouches.
	// * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	// * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	// */
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStartCrouch", ScriptName = "OnStartCrouch"))
	//void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);



	///** Set by character movement to specify that this Character is currently crouched. This function returns bIsCrouched.*/
	//bool IsProned() const;
	///** Specifies whether this Character is currently crouched or not. This function sets bIsCrouched and marks property as dirty.*/
	//void SetIsProneed(const bool bInIsProned);
};
