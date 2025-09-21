// Kakanu Portfolio Project

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "PUBGDefine.h"
#include "PUBGFunctionLibrary.generated.h"

class UPUBGAbilitySystemComponent;
class ACharacter;
/**
 * 
 */
UCLASS(BlueprintType)
class PUBG_API UPUBGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// c++ 컴포넌트 들고오기
	static UPUBGAbilitySystemComponent* NativeGetPUBGASCFromActor(AActor* InActor);

	// 액터에 Tag 추가
	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	// 액터에 Tag 삭제
	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	// 액터 Tag 소유 여부 확인
	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EPUBGConfirmType& OutConfirmType);

	static void NativeSetMoveSpeed(ACharacter* InCharacter);

	static void NativeSetMoveSpeedByState(ACharacter* InCharacter, FString NextState);
};
