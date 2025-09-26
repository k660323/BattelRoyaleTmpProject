// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "PUBGEquipmentBase.generated.h"

class UArrowComponent;
class UBoxComponent;
class UPUBGItemInstance;

UCLASS()
class PUBG_API APUBGEquipmentBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APUBGEquipmentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void Init(int32 InItemTemplateID, EEquipmentSlotType InEquipmentSlotType);

	// ���� ���� ���� ����
	bool IsEquipableWeaponAccTypeSocket(EGunAccType WeaponAccType);

	// ���� ������ �ν��Ͻ� ����
	bool IsValidWeaponAcc(EGunAccType WeaponAccType);

	UPUBGItemInstance* GetWeaponAccItemInstance(EGunAccType WeaponAccType);

	// ���� �׼�����
	bool EquipmentWeaponAcc(EGunAccType WeaponAccType, UPUBGItemInstance* InItemInstance);
	// Ż�� �׼�����
	UPUBGItemInstance* UnEquipmentWeaponAcc(EGunAccType WeaponAccType);

public:
	// IAbilitySystemInterface Begin
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// IAbilitySystemInterface End

	EEquipmentSlotType GetEquipmentSlotType() const;
	int32 GetItemTempalteID() const;

	EWeaponClassType GetWeaponClassType() const;

	USkeletalMeshComponent* GetTPSEquipmentMesh() const;
	USkeletalMeshComponent* GetFPSEquipmentMesh() const;

	UBoxComponent* GetTraceDebugCollision() const;

private:
	UFUNCTION()
	void OnRep_EquipmentSlotType();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> TPSMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FPS_Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TraceDebugCollision;

protected:
	UPROPERTY(Replicated)
	int32 ItemTemplateID;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentSlotType)
	EEquipmentSlotType EquipmentSlotType;
};
