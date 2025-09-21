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
	// c++ ������Ʈ ������
	static UPUBGAbilitySystemComponent* NativeGetPUBGASCFromActor(AActor* InActor);

	// ���Ϳ� Tag �߰�
	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	// ���Ϳ� Tag ����
	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	// ���� Tag ���� ���� Ȯ��
	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "PUBG/FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EPUBGConfirmType& OutConfirmType);

	static void NativeSetMoveSpeed(ACharacter* InCharacter);

	static void NativeSetMoveSpeedByState(ACharacter* InCharacter, FString NextState);
};
