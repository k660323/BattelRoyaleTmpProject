// Fill out your copyright notice in the Description page of Project Settings.


#include "System/PUBGGameInstance.h"
#include "System/PUBGAssetManager.h"

UPUBGGameInstance::UPUBGGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPUBGGameInstance::Init()
{
	Super::Init();

	// ���� �ʱ�ȭ
	UPUBGAssetManager::Get().Initialize();
}

void UPUBGGameInstance::Shutdown()
{
	Super::Shutdown();

}
