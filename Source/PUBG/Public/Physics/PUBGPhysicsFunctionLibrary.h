// Kakanu Portfolio Project

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PUBGPhysicsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGPhysicsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary")
	static bool SphereIntersectsBox(const FVector& SphereCenter, float SphereRadius, const FBox& Box);

};
