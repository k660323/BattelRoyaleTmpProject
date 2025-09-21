// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Components/ActorComponent.h"
#include "PUBGCosmeticManagerComponent.generated.h"

class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUBG_API UPUBGCosmeticManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPUBGCosmeticManagerComponent();
	virtual void InitializeComponent() override;

public:
	void InitSkeletalMesh();
	void ReplaceSkeletalMesh(EFashionType FashionType, USkeletalMesh* InSkeletalMesh, UTexture* InMaskTexture);
	void ReplaceArmorMesh(EArmorType ArmorType, UStaticMesh* InArmorMesh, FName SocketName);
	void ReplaceUtilityMesh(EUtilityType UtilityType, UStaticMesh* InUtilityMesh, FName SocketName);

	ACharacter* GetOwnerCharacter() const;
private:
	UPROPERTY(Category = PUBGCharacter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<USkeletalMeshComponent>> FashionMeshs;

	UPROPERTY(Category = PUBGCharacter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UStaticMeshComponent>> ArmorMeshs;

	UPROPERTY(Category = PUBGCharacter, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UStaticMeshComponent>> UtilityMeshs;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* SkinMatRef;

	bool bInitializedSkeletalMesh = false;
};
