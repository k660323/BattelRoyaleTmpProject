// Kakanu Portfolio Project

#pragma once

UENUM()
enum class EPUBGConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class EPUBGValidType : uint8
{
	Valid,
	Invalid
};

UENUM()
enum class EPUBGSuccessType : uint8
{
	Successful,
	Failed
};

UENUM()
enum class EPUBGInputMode : uint8
{
	GameOnly,
	UIOnly
};

UENUM(BlueprintType)
enum class EPUBGDirection : uint8
{
	None,
	Forward,
	Right,
	Backward,
	Left,

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Armor,
	Weapon,
	Fashion,
	Utility,
	GunAccessory,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Unarmed,
	Main,
	Main2,
	Sub,
	Etc,
	Throw,
	Helmet,
	Vest,
	Belt,
	Backpack,
	Cap,
	ClothTop,
	ClothBottom,
	Whole,
	Shoes,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EActiveWeaponEquipmentType : uint8
{
	Unarmed,
	Main,
	Main2,
	Sub,
	Etc,
	Throw,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	Main,
	Sub,
	ETC,
	Throw,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponClassType : uint8
{
	None,							// ¾øÀ½
	Melee,							// ±ÙÁ¢ ¹«±â
	Pistols,						// ±ÇÃÑ
	ShotGuns,						// »êÅºÃÑ
	SubMachineGuns,					// ±â°ü´ÜÃÑ (SMG)
	RifleAndMachineguns,			// ¼ÒÃÑ ¹× ±â°üÃÑ
	MarksmanRifles,					// Á¤¹Ð »ç°Ý ¼ÒÃÑ (DMR)
	SniperRifles,					// Àú°ÝÃÑ (SR)

	Count UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Helmet,
	Vest,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EConsumptionType : uint8
{
	Heal,
	Drink,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPayloadType : uint8
{
	Ammo,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Ammo_556mm,
	Ammo_776mm,
	Ammo_9mm,
	Ammo_45ACP,
	Ammo_12Gauge,
	Ammo_300Mag,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EFashionType : uint8
{
	Cap,
	Hair,
	ClothTop,
	ClothBottom,
	Whole,
	Shoes,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EGunAccType : uint8
{
	Sight,
	Muzzle,
	Foregrip,
	Mag,
	Buttstock,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EUtilityType : uint8
{
	Backpack,
	Belt,

	Count UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EFiringMode : uint8
{
	Shot,
	Burst,
	Auto,

	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEtcType : uint8
{
	None,
	Count UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon, // weapon fragment
	Armor,  // armor fragment
	Consumption, // consumption fragment
	Fashion, // fashion fragment
	GunAcc, // gunacc fragment
	Utility, // utility fragment
	Payload, // Payload fragment
	ETC, // etc fragment

	Count UMETA(Hidden)
};