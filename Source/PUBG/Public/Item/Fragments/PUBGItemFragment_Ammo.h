// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Item/Fragments/PUBGItemFragment_Payload.h"
#include "PUBGItemFragment_Ammo.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Ammo : public UPUBGItemFragment_Payload
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Ammo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EAmmoType AmmoType;
};
