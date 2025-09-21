//// Kakanu Portfolio Project


#include "Item/FeatureConditions/ConsumeCondition/PUBGFeatureCondition_Heal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PUBGVitalSet.h"

 #include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGFeatureCondition_Heal)

UPUBGFeatureCondition_Heal::UPUBGFeatureCondition_Heal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UPUBGFeatureCondition_Heal::CanUseFeature(UObject* Owner) const
{
	if (Owner == nullptr)
	{
		return false;
	}

	if (TargetAttribute.IsValid() == false)
	{
		return false;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Owner);
	if (AbilitySystemInterface == nullptr)
	{
		return false;
	}

	
	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return false;
	}

	// Attribute ���� ��ȿ���� ������ ���������� ����
	float Value = ASC->GetNumericAttributeChecked(TargetAttribute);

	return (Value < Limit);
}
