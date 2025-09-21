// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 로컬
#define LOG_LOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
// 원격
#define LOG_REMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))

// 로컬
#define LOG_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
// 원격
#define LOG_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))

// 로컬
#define LOG_ABILITY_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetAvatarActorFromActorInfo()->GetLocalRole()))
// 원격
#define LOG_ABILITY_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetAvatarActorFromActorInfo()->GetRemoteRole()))



// 네트워크 모드를 표현하는 매크로
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), static_cast<int>(GPlayInEditorID)) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 
#define LOG_ABILITY_NETMODEINFO ((GetAvatarActorFromActorInfo()->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), static_cast<int>(GPlayInEditorID)) : ((GetAvatarActorFromActorInfo()->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 

// 로그를 출력하는 클래스를 출력하는 매크로
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

// 커스텀 로그
#define PUBG_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] [%s/%s] %s %s"), LOG_NETMODEINFO, LOG_LOCALROLEINFO, LOG_REMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define PUBG_SUBLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] [%s/%s] %s %s"), LOG_NETMODEINFO, LOG_SUBLOCALROLEINFO, LOG_SUBREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define PUBG_ABILITY_SUBLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] [%s/%s] %s %s"), LOG_ABILITY_NETMODEINFO, LOG_ABILITY_SUBLOCALROLEINFO, LOG_ABILITY_SUBREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(LogPUBGNetwork, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogPUBGTeleport, Log, All);