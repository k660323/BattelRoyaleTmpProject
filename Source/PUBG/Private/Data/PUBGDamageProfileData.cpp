// Kakanu Portfolio Project


#include "Data/PUBGDamageProfileData.h"

#if WITH_EDITORONLY_DATA
#include "UObject/ObjectSaveContext.h"
#endif // WITH_EDITORONLY_DATA

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGDamageProfileData)

UPUBGDamageProfileData::UPUBGDamageProfileData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultDamageProfiles.SetNum(12);

	for (int32 i = 0; i < DefaultDamageProfiles.Num(); i++)
	{
		DefaultDamageProfiles[i].PhysicalSurface = static_cast<EPhysicalSurface>(i + 1);
	}

	WeaponDamageProfiles.SetNum(static_cast<int32>(EWeaponClassType::Count));
	for (int32 i = 0; i < WeaponDamageProfiles.Num(); i++)
	{
		WeaponDamageProfiles[i].WeaponClassType = static_cast<EWeaponClassType>(i);

		TArray<FPUBGDamageProfile>& DamageProfiles = WeaponDamageProfiles[i].DamageProfiles;
		DamageProfiles.SetNum(12);

		for (int32 j = 0; j < DamageProfiles.Num(); j++)
		{
			DamageProfiles[j].PhysicalSurface = static_cast<EPhysicalSurface>(j + 1);
		}
	}
}

#if WITH_EDITORONLY_DATA
void UPUBGDamageProfileData::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);

	DefaultDamageProfileMap.Empty();

	for (FPUBGDamageProfile& DamageProfile : DefaultDamageProfiles)
	{
		if (DefaultDamageProfileMap.Contains(DamageProfile.PhysicalSurface))
		{
			UE_LOG(LogTemp, Warning, TEXT("UPUBGDamageProfileData::PreSave - DefaultDamageProfiles Duplicate PhysicalSurface Enum : %d"), DamageProfile.PhysicalSurface.GetValue());
			continue;
		}
		DefaultDamageProfileMap.Add(DamageProfile.PhysicalSurface, DamageProfile);
	}

	WeaponDamageProfileMap.Empty();

	for (FPUBGWeaponDamageProfile& WeaponDamageProfile : WeaponDamageProfiles)
	{
		if (WeaponDamageProfileMap.Contains(WeaponDamageProfile.WeaponClassType))
		{
			const UEnum* EnumPtr = StaticEnum<EWeaponClassType>();
			UE_LOG(LogTemp, Warning, TEXT("UPUBGDamageProfileData::PreSave - WeaponDamageProfiles Duplicate WeaponClassType Enum : %s"), *(EnumPtr->GetNameStringByValue(static_cast<int64>(WeaponDamageProfile.WeaponClassType))));
			continue;
		}

		WeaponDamageProfileMap.Add(WeaponDamageProfile.WeaponClassType, WeaponDamageProfile);
	}

}
#endif // WITH_EDITORONLY_DATA

float UPUBGDamageProfileData::GetDefaultDamageProfileRate(TEnumAsByte<EPhysicalSurface> PhysicalSurface) const
{
	return DefaultDamageProfileMap[PhysicalSurface].DamateRate;
}

float UPUBGDamageProfileData::GetWeaponDamageProfileRate(EWeaponClassType WeaponClassType, TEnumAsByte<EPhysicalSurface> PhysicalSurface) const
{
	return WeaponDamageProfileMap[WeaponClassType].DamageProfiles[PhysicalSurface].DamateRate;
}
