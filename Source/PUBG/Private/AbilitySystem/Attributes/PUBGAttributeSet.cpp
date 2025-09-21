// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/PUBGAttributeSet.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAttributeSet)

UPUBGAttributeSet::UPUBGAttributeSet()
{


}

UWorld* UPUBGAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UPUBGAbilitySystemComponent* UPUBGAttributeSet::GetPUBGAbilitySystemComponent() const
{
	return Cast<UPUBGAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}