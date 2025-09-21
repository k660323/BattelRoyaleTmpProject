// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PUBGInputData.generated.h"

class UInputAction;
class UInputMappingContext;

// �׼� + ��ǲ�� �ϳ��� ���� ����ü
USTRUCT(BlueprintType)
struct FPUBGInputAction
{
	GENERATED_BODY()

public:
	FPUBGInputAction() :
		InputTag(FGameplayTag::EmptyTag),
		InputAction(nullptr)
	{ }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction;

public:
	bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}
};

USTRUCT(BlueprintType)
struct FPUBGInputMappingContextData
{
	GENERATED_BODY()

public:
	FPUBGInputMappingContextData() : InputMappingContext(nullptr), Priority(0) {}

public:
	// ����ڷκ��� �Է��� �޴� ��ü
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Priority;

public:
	bool IsValid() const
	{
		return (InputMappingContext != nullptr);
	}
};

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGInputData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction* FindInputActionByTag(const FGameplayTag& InputTag) const;

public:
	// virtual void PostLoad() override;
	
	// �⺻ �Է�--------------------------
	// ����ڷκ��� �Է��� �޴� ��ü
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FPUBGInputMappingContextData NativeInputMappingContextData;

	// �⺻ (�׼ǰ� �±װ� ��Ʈ�� �̷���� ��ü���� TArray��) (�Լ� ���ε� �ʼ�)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FPUBGInputAction> NativeInputActions;
	//-------------------------------------
	 
	// Ư�� �Է�---------------------------
	// ��ų ���� (�׼ǰ� �±װ� ��Ʈ�� �̷���� ��ü���� TArray��) (�Լ� ���ε� �ȵɼ� ����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FPUBGInputAction> AbilityInputActions;
	//-------------------------------------
};
