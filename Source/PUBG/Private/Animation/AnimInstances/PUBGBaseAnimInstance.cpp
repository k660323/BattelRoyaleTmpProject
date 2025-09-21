// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstances/PUBGBaseAnimInstance.h"
#include "Utils/PUBGFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGBaseAnimInstance)

bool UPUBGBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if(APawn* OwningPawn = TryGetPawnOwner())
	{
		return UPUBGFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
	}

	return false;
}
