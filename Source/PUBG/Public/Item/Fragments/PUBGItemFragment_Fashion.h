// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "PUBGItemFragment_Fashion.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Fashion : public UPUBGItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Fashion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditDefaultsOnly)
	EFashionType FashionType = EFashionType::Count;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> FashionMesh;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture> MaskTexture;
};
