// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PUBGInputData.h"

const UInputAction* UPUBGInputData::FindInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FPUBGInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Can't find InputAction for InputTag [%s]"), *InputTag.ToString());

	return nullptr;
}
