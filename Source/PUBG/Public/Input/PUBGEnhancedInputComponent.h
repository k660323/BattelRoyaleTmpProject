// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Data/PUBGInputData.h"
#include "PUBGEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UPUBGInputData* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);

	template<class UserObject, typename StartedFuncType, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityInputAction(const UPUBGInputData* InInputConfig, UserObject* ContextObject, StartedFuncType InputStartedFunc, PressedFuncType InputPressedFunc, ReleasedFuncType InputReleasedFunc);

};


template<class UserObject, typename CallbackFunc>
inline void UPUBGEnhancedInputComponent::BindNativeInputAction(const UPUBGInputData* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig, TEXT("Input config data asset is null, can not proceed with binding"));

	if (const UInputAction* FoundAction = InInputConfig->FindInputActionByTag(InInputTag))
	{
		BindAction(FoundAction, TriggerEvent, ContextObject, Func);
	}

}

template<class UserObject, typename StartedFuncType, typename PressedFuncType, typename ReleasedFuncType>
inline void UPUBGEnhancedInputComponent::BindAbilityInputAction(const UPUBGInputData* InInputConfig, UserObject* ContextObject, StartedFuncType InputStartedFunc, PressedFuncType InputPressedFunc, ReleasedFuncType InputReleasedFunc)
{
	checkf(InInputConfig, TEXT("Input config data asset is null, can not proceed with binding"));

	//TArray<FInputBindingHandle> BindingHandles;

	for (const FPUBGInputAction& AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid())
		{
			continue;
		}
	
		// 인풋 액션, 트리거 이벤트, 대상 오브젝트, 콜백함수, 콜백 함수 매개변수
		if (InputStartedFunc)
		{
			FInputBindingHandle BindHandle = BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputStartedFunc, AbilityInputActionConfig.InputTag);
			//BindingHandles.Add(BindHandle);
		}

		if (InputPressedFunc)
		{
			FInputBindingHandle BindHandle = BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Triggered, ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag);
			//BindingHandles.Add(BindHandle);
		}
		
		if (InputReleasedFunc)
		{
			FInputBindingHandle BindHandle = BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputReleasedFunc, AbilityInputActionConfig.InputTag);
			//BindingHandles.Add(BindHandle);
		}
	}
}
