// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PUBGInputData.generated.h"

class UInputAction;
class UInputMappingContext;

// 액션 + 인풋을 하나로 묶은 구조체
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
	// 사용자로부터 입력을 받는 객체
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
	
	// 기본 입력--------------------------
	// 사용자로부터 입력을 받는 객체
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FPUBGInputMappingContextData NativeInputMappingContextData;

	// 기본 (액션과 태그가 세트로 이루어진 객체들의 TArray들) (함수 바인딩 필수)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FPUBGInputAction> NativeInputActions;
	//-------------------------------------
	 
	// 특수 입력---------------------------
	// 스킬 전용 (액션과 태그가 세트로 이루어진 객체들의 TArray들) (함수 바인딩 안될수 있음)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FPUBGInputAction> AbilityInputActions;
	//-------------------------------------
};
