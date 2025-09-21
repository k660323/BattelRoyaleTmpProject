// Kakanu Portfolio Project

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PUBGAttributeSet.h"
#include "PUBGInventorySet.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGInventorySet : public UPUBGAttributeSet
{
	GENERATED_BODY()
	
public:
	UPUBGInventorySet();

	// ���ø����̼� ���� ���
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

public:
	// Effect ������
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue);
	
private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

public:
	ATTRIBUTE_ACCESSORS(ThisClass, AdditionalMaxCapacityWeight);

protected:
	UFUNCTION()
	void OnRep_AdditionalMaxCapacityWeight(const FGameplayAttributeData& OldValue);

private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AdditionalMaxCapacityWeight, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData AdditionalMaxCapacityWeight;
};
