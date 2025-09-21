// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PUBGGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUBG_API UPUBGGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPUBGGameInstance(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Init() override;
	virtual void Shutdown() override;
};
