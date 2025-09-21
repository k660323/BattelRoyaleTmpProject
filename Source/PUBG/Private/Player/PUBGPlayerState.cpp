// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PUBGPlayerState.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/PUBGVitalSet.h"
#include "AbilitySystem/Attributes/PUBGCombatSet.h"
#include "AbilitySystem/Attributes/PUBGInventorySet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGPlayerState)

APUBGPlayerState::APUBGPlayerState()
{
	// ��Ʈ������ �÷��̾��� GAS ��ɵ��� ���⼭ �ʱ�ȭ �Ѵ�.
	// ���� : �ʱ�ȭ Ÿ�̹��� �ٸ��� ���� (Ŭ��)
	PUBGAbilitySystemComponent = CreateDefaultSubobject<UPUBGAbilitySystemComponent>(TEXT("PUBGAbilitySystemComponent"));
	PUBGAbilitySystemComponent->SetIsReplicated(true);
	PUBGAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PUBGVitalSet = CreateDefaultSubobject<UPUBGVitalSet>(TEXT("VitalSet"));

	PUBGCombatSet = CreateDefaultSubobject<UPUBGCombatSet>(TEXT("CombatSet"));

	PUBGInventorySet = CreateDefaultSubobject<UPUBGInventorySet>(TEXT("InventorySet"));
	
	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* APUBGPlayerState::GetAbilitySystemComponent() const
{
	return GetPUBGAbilitySystemComponent();
}

UPUBGAbilitySystemComponent* APUBGPlayerState::GetPUBGAbilitySystemComponent() const
{
	return PUBGAbilitySystemComponent;
}

UPUBGVitalSet* APUBGPlayerState::GetPUBGVitalSetComponent() const
{
	return PUBGVitalSet;
}

UPUBGCombatSet* APUBGPlayerState::GetPUBGCombatSetComponent() const
{
	return PUBGCombatSet;
}

UPUBGInventorySet* APUBGPlayerState::GetPUBGInventorySetComponent() const
{
	return PUBGInventorySet;
}
