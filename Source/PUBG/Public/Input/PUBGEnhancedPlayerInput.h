// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "EnhancedPlayerInput.h"
#include "PUBGEnhancedPlayerInput.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGEnhancedPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()
	
public:
	UPUBGEnhancedPlayerInput();

public:
	void FlushPressedInput(UInputAction* InputAction);
};
