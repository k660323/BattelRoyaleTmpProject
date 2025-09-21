// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PUBGPickupable.generated.h"

class UPUBGItemInstance;

USTRUCT(BlueprintType)
struct FPUBGPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPUBGItemInstance> ItemInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount = 1;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPUBGPickupable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PUBG_API IPUBGPickupable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual FPUBGPickupInstance GetPickupInfo() const = 0;
};
