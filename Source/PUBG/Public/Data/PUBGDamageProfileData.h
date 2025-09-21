// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Engine/DataAsset.h"
#include "PUBGDamageProfileData.generated.h"

USTRUCT(BlueprintType)
struct FPUBGDamageProfile : public FTableRowBase
{
	GENERATED_BODY()
public:
	FPUBGDamageProfile()
		: PhysicalSurface(EPhysicalSurface::SurfaceType_Default), DamateRate(1.0f) {
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EPhysicalSurface> PhysicalSurface;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamateRate;
};

USTRUCT(BlueprintType)
struct FPUBGWeaponDamageProfile : public FTableRowBase
{
	GENERATED_BODY()
public:
	FPUBGWeaponDamageProfile()
		: WeaponClassType(EWeaponClassType::Count), DamageProfiles(){
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponClassType WeaponClassType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPUBGDamageProfile> DamageProfiles;
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGDamageProfileData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPUBGDamageProfileData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 저장시 캐싱 변수에 데이터를 삽입 
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
	
	// 기본 데미지 (1)
	float GetDefaultDamageProfileRate(TEnumAsByte<EPhysicalSurface> PhysicalSurface) const;

	// 무기 데미지 (2)
	float GetWeaponDamageProfileRate(EWeaponClassType WeaponClassType, TEnumAsByte<EPhysicalSurface> PhysicalSurface) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, EditFixedSize, Category = "DamageProfile", meta = (AllowPrivateAccess ="true"))
	TArray<FPUBGDamageProfile> DefaultDamageProfiles;

	UPROPERTY()
	TMap<TEnumAsByte<EPhysicalSurface>, FPUBGDamageProfile> DefaultDamageProfileMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageProfile", meta = (AllowPrivateAccess = "true"))
	TArray<FPUBGWeaponDamageProfile> WeaponDamageProfiles;

	UPROPERTY()
	TMap<EWeaponClassType, FPUBGWeaponDamageProfile> WeaponDamageProfileMap;
};
