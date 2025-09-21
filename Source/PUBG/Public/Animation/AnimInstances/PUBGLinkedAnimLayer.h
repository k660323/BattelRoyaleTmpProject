// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstances/PUBGBaseAnimInstance.h"
#include "PUBGLinkedAnimLayer.generated.h"

class UPUBGCharacterAnimInstance;
/**
 * 
 */
UCLASS()
class PUBG_API UPUBGLinkedAnimLayer : public UPUBGBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UPUBGCharacterAnimInstance* GetCharacterAnimInstance() const;
};
