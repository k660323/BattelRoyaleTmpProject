// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PUBGPickupItemManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UPUBGPickupItemManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGPickupItemManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

};
