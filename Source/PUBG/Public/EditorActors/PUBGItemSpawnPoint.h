// Kakanu Portfolio Project

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityActor.h"
#include "PUBGItemSpawnPoint.generated.h"

class USphereComponent;
class UArrowComponent;
class USceneComponent;
/**
 * 
 */
UCLASS()
class PUBG_API APUBGItemSpawnPoint : public AEditorUtilityActor
{
	GENERATED_BODY()
	
public:
	APUBGItemSpawnPoint();

public:
	TArray<FTransform> GetItemSpawnWorldTransform();
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UArrowComponent> VisualSpawnArrowMark;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SpawnRangeCollisionComponent;
};
