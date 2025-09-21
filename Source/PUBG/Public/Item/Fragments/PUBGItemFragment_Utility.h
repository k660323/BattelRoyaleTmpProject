// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Item/Fragments/PUBGItemFragment_Equipable.h"
#include "PUBGItemFragment_Utility.generated.h"


/**
 * 
 */
UCLASS()
class PUBG_API UPUBGItemFragment_Utility : public UPUBGItemFragment_Equipable
{
	GENERATED_BODY()
	
public:
	UPUBGItemFragment_Utility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void OnInstanceCreated(UPUBGItemInstance* ItemInstance) const override;

public:
	UPROPERTY(EditDefaultsOnly)
	EUtilityType UtilityType;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> UtilityMesh;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Thumbnails;

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow))
	TArray<FStatSet> StatSets;
};
