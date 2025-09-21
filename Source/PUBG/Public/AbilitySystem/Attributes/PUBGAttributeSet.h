// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AttributeSet.h"
#include "PUBGAttributeSet.generated.h"

class UPUBGAbilitySystemComponent;
struct FGameplayEffectSpec;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// FGameplayAttributeData 변경시 콜백 호출할 델리게이트 선언
DECLARE_MULTICAST_DELEGATE_SixParams(FPUBGAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPUBGAttributeSet();
	
	UWorld* GetWorld() const override;
	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const;
};
