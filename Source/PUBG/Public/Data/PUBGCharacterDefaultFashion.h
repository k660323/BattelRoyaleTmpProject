// Kakanu Portfolio Project

#pragma once

#include "PUBGDefine.h"
#include "Engine/DataAsset.h"
#include "PUBGCharacterDefaultFashion.generated.h"

USTRUCT(BlueprintType)
struct FPUBGCharacterDefaultFashionData
{
	GENERATED_BODY()
	
public:
	FPUBGCharacterDefaultFashionData() 
	: FashionType(EFashionType::Count), ItemTempalteID(0) {};

	bool IsValid() const { return ItemTempalteID > 0; };

public:
	UPROPERTY(VisibleDefaultsOnly)
	EFashionType FashionType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemTempalteID;
};

UCLASS()
class PUBG_API UPUBGCharacterDefaultFashion : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPUBGCharacterDefaultFashion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif // WITH_EDITORONLY_DATA

public:
	UTexture* GetDefaultMaskTexture() const;

	void GetFashionMesh(EFashionType InFashionType, USkeletalMesh*& OutSkeletalMesh, UTexture*& OutTexture) const;

	FPUBGCharacterDefaultFashionData GetFashionData(EFashionType InFashionType);
private:
	UPROPERTY(EditDefaultsOnly, Category = "DefaultFashion")
	UTexture* DefaultMaskTexture;

	UPROPERTY(EditAnywhere, EditFixedSize, Category = "DefaultFashion")
	TArray<FPUBGCharacterDefaultFashionData> DefaultFashionDatas;

	UPROPERTY()
	TMap<EFashionType, FPUBGCharacterDefaultFashionData> DefaultFashionDataMap;
};
