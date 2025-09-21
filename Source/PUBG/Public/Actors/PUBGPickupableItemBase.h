// Kakanu Portfolio Project

#pragma once

#include "GameFramework/Actor.h"
#include "Interaction/PUBGInteractable.h"
#include "Interaction/PUBGPickupable.h"
#include "PUBGPickupableItemBase.generated.h"

class SceneComponent;
class UBoxComponent;

UCLASS(BlueprintType)
class PUBG_API APUBGPickupableItemBase : public AActor, public IPUBGInteractable, public IPUBGPickupable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APUBGPickupableItemBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

protected:
	void DistanceCulling();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetPickupInfo(const FPUBGPickupInstance& InPickupInfo);

	// IPUBGInteractable Interface Begin
	virtual FPUBGInteractionInfo GetPreInteractionInfo(const FPUBGInteractionQuery& InteractionQuery) const { return InteractionInfo; };
	virtual void GetMeshComponents(TArray<UMeshComponent*>& OutMeshComponents) const;

	// IPUBGInteractable Interface End
	
	// IPUBGPickupable Interface Begin
	virtual FPUBGPickupInstance GetPickupInfo() const override { return PickupInfo; };
	// IPUBGPickupable Interface End
	
	UBoxComponent* GetCollisionBoxComponent() const;

public:
	UFUNCTION()
	virtual void OnRep_PickupInfo();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> PickupCollision;

	UPROPERTY(EditAnyWhere, Category = "Info")
	FPUBGInteractionInfo InteractionInfo;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_PickupInfo, Category = "Info")
	FPUBGPickupInstance PickupInfo;

private:
	FTimerHandle CullingDistanceTimerHandle;
	float CullingDistanceSquared;
};
