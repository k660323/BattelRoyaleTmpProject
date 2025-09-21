// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PUBGVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FPUBGVerbMessage;

class APlayerState;
class APlayerController;
class UObject;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "PUBG")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "PUBG")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "PUBG")
	static FGameplayCueParameters VerbMessageToCueParameters(const FPUBGVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "PUBG")
	static FPUBGVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
