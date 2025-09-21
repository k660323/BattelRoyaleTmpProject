// Kakanu Portfolio Project


#include "EditorActors/PUBGItemSpawnPoint.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGItemSpawnPoint)



APUBGItemSpawnPoint::APUBGItemSpawnPoint()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
	bIsEditorOnlyActor = true;
	bNetLoadOnClient = false;
	SetReplicates(false);
	SetReplicateMovement(false);
	SetHidden(true);

	SceneComponent =CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	VisualSpawnArrowMark = CreateDefaultSubobject<UArrowComponent>(TEXT("VisualSpawnArrowMark"));
	VisualSpawnArrowMark->SetupAttachment(SceneComponent);
	VisualSpawnArrowMark->SetCollisionProfileName(TEXT("NoCollision"));
	VisualSpawnArrowMark->PrimaryComponentTick.bStartWithTickEnabled = false;
	VisualSpawnArrowMark->PrimaryComponentTick.bCanEverTick = false;
	VisualSpawnArrowMark->SetHiddenInGame(true);
	VisualSpawnArrowMark->SetGenerateOverlapEvents(false);
	VisualSpawnArrowMark->SetEnableGravity(false);
	VisualSpawnArrowMark->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	VisualSpawnArrowMark->ArrowSize = 1.5f;
	VisualSpawnArrowMark->ArrowLength = 100.0f;
	VisualSpawnArrowMark->ArrowColor = FColor::Green;

	SpawnRangeCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnRangeCollision"));
	SpawnRangeCollisionComponent->SetupAttachment(SceneComponent);
	SpawnRangeCollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));
	SpawnRangeCollisionComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	SpawnRangeCollisionComponent->PrimaryComponentTick.bCanEverTick = false;
	SpawnRangeCollisionComponent->SetIsReplicated(false);
	SpawnRangeCollisionComponent->SetHiddenInGame(true);
	SpawnRangeCollisionComponent->SetSphereRadius(150.0f);
	SpawnRangeCollisionComponent->SetVisibility(true);
	SpawnRangeCollisionComponent->SetGenerateOverlapEvents(false);
	SpawnRangeCollisionComponent->SetEnableGravity(false);
	SpawnRangeCollisionComponent->bHiddenInGame = true;

	SpawnRangeCollisionComponent->bVisibleInReflectionCaptures = false;
	SpawnRangeCollisionComponent->bVisibleInRayTracing = false;
	SpawnRangeCollisionComponent->bVisibleInRealTimeSkyCaptures = false;
	SpawnRangeCollisionComponent->bVisibleInSceneCaptureOnly = false;

}

TArray<FTransform> APUBGItemSpawnPoint::GetItemSpawnWorldTransform()
{
	TArray<FTransform> ItemSpawnWorldTransform;
	ItemSpawnWorldTransform.SetNum(5);


	FTransform transform = GetActorTransform();
	ItemSpawnWorldTransform[0] = transform;

	transform.SetLocation(GetActorLocation() + FVector(100.0f, 0.0f, 0.0f));
	transform.SetRotation(FQuat(GetActorRotation()));
	ItemSpawnWorldTransform[1] = transform;


	transform.SetLocation(GetActorLocation() + FVector(-80.0f, 60.0f, 0.0f));
	transform.SetRotation(FQuat(GetActorRotation() + FRotator(0.0f, 180.0f, 0.0f)));
	ItemSpawnWorldTransform[2] = transform;


	transform.SetLocation(GetActorLocation() + FVector(-30.0f, -80.0f, 0.0f));
	transform.SetRotation(FQuat(GetActorRotation() + FRotator(0.0f, -90.0f, 0.0f)));
	ItemSpawnWorldTransform[3] = transform;


	transform.SetLocation(GetActorLocation() + FVector(40.0f, 140.0f, 0.0f));
	transform.SetRotation(FQuat(GetActorRotation() + FRotator(0.0f, 90.0f, 0.0f)));
	ItemSpawnWorldTransform[4] = transform;

	return ItemSpawnWorldTransform;
}

