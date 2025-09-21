// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "PUBGPlayerState.generated.h"

class UPUBGAbilitySystemComponent;
class UPUBGVitalSet;
class UPUBGCombatSet;
class UPUBGInventorySet;
/**
 * 
 */
UCLASS()
class PUBG_API APUBGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	APUBGPlayerState();

	// IAbilitySystemInterface 인터페이스
	// 외부에서 어빌리티 컴포넌트 접근 함수
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// -----

	UPUBGAbilitySystemComponent* GetPUBGAbilitySystemComponent() const;

	UPUBGVitalSet* GetPUBGVitalSetComponent() const;

	UPUBGCombatSet* GetPUBGCombatSetComponent() const;

	UPUBGInventorySet* GetPUBGInventorySetComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	UPUBGAbilitySystemComponent* PUBGAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilitySystem|Attribute")
	UPUBGVitalSet* PUBGVitalSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilitySystem|Attribute")
	UPUBGCombatSet* PUBGCombatSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AbilitySystem|Attribute")
	UPUBGInventorySet* PUBGInventorySet;
};
