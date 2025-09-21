// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Engine/DataTable.h"
#include "PUBGItemProbability.generated.h"

USTRUCT(BlueprintType)
struct FPUBGItemCountProbability : public FTableRowBase
{
	GENERATED_BODY()
public:
	FPUBGItemCountProbability()
		: Percent(0.0f), ItemCount(0) {}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemCount;
};


USTRUCT(BlueprintType)
struct FPUBGItemTypeProbability : public FTableRowBase
{
	GENERATED_BODY()
public:
	FPUBGItemTypeProbability()
		: ItemType(EItemType::Count), Percent(0.0f) {}
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGMiddleClassification : public FTableRowBase
{
	GENERATED_BODY()

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() 
	{
		return TPair<UEnum*, int32>();
	};

	virtual float GetPercent()
	{
		return 0.0f;
	};
};

USTRUCT(BlueprintType)
struct FPUBGWeaponTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()
	
public:
	FPUBGWeaponTypeProbability()
		: WeaponType(EWeaponType::Count), Percent(0.0f) {}

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EWeaponType>(), (int32)WeaponType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGGunAccTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGGunAccTypeProbability()
	: GunAccType(EGunAccType::Count), Percent(0.0f) {}

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EGunAccType>(), (int32)GunAccType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EGunAccType GunAccType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGArmorTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGArmorTypeProbability()
		: ArmorType(EArmorType::Count), Percent(0.0f) {}

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EArmorType>(), (int32)ArmorType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EArmorType ArmorType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGConsumptionTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGConsumptionTypeProbability()
		: ConsumptionType(EConsumptionType::Count), Percent(0.0f) {}

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EConsumptionType>(), (int32)ConsumptionType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EConsumptionType ConsumptionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGFashionTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGFashionTypeProbability()
	: FashionType(EFashionType::Count), Percent(0.0f) { }

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EFashionType>(), (int32)FashionType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFashionType FashionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGUtilityTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGUtilityTypeProbability()
	: UtilityType(EUtilityType::Count), Percent(0.0f) { }

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EUtilityType>(), (int32)UtilityType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EUtilityType UtilityType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGAmmoTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGAmmoTypeProbability()
	: PayloadType(EPayloadType::Count), Percent(0.0f) {}

public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EPayloadType>(), (int32)PayloadType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPayloadType PayloadType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGEtcTypeProbability : public FPUBGMiddleClassification
{
	GENERATED_BODY()

public:
	FPUBGEtcTypeProbability()
		: EtcType(EEtcType::Count), Percent(0.0f) {}
public:
	virtual TPair<UEnum*, int32> GetEnumInfoPair() override
	{
		return TPair<UEnum*, int32>(StaticEnum<EEtcType>(), (int32)EtcType);
	};

	virtual float GetPercent() override
	{
		return Percent;
	};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEtcType EtcType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};

USTRUCT(BlueprintType)
struct FPUBGItemModelProbability
{
	GENERATED_BODY()

public:
	FPUBGItemModelProbability() 
	: DisplayName(FText()), IconTexture(nullptr), ItemNumber(0), Percent(0.0f) {}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemNumber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Percent;
};