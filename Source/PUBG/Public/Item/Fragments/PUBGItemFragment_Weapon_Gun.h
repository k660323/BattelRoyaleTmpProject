// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Item/Fragments/PUBGItemFragment_Weapon.h"
#include "PUBGItemFragment_Weapon_Gun.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Weapon_Gun : public UPUBGItemFragment_Weapon
{
	GENERATED_BODY()

public:
	UPUBGItemFragment_Weapon_Gun(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Gun")
	int32 ClipSize;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Gun")
	TSoftObjectPtr<UCurveFloat> DistanceDamageCurve;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Gun")
	float FiringInterval;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Gun")
	float ReplaceClipTime;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Gun")
	float ReplaceBulletTime;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Gun")
	TMap<int32, EFiringMode> FiringMode;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|GunAcc")
	TSoftObjectPtr<UStaticMesh> DefaultMag;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|GunAcc")
	TSoftObjectPtr<UStaticMesh> DefaultSight;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|GunAcc")
	TSoftObjectPtr<UStaticMesh> DefaultButtstock;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Sound")
	TSoftObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Sound")
	TSoftObjectPtr<USoundBase> FirSoundSuppressor;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMin_Sillness;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMax_Sillness;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMin_Move;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMax_Move;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMin_AimStillness;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMax_AimStillness;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMin_AimMove;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|CrossOffset")
	float CrossOffsetMax_AimMove;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|ReBound")
	float VerticalOffset;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|ReBound")
	float HorizontalOffset;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Bullet")
	TSubclassOf<UPUBGItemTemplate> AmmoItemTemplateClass;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Bullet")
	float BulletVelocity;

	UPROPERTY(EditDefaultsOnly, Category = "PUBGItemFragment_Weapon_Gun|Bullet")
	float BulletGravityScale;
};
