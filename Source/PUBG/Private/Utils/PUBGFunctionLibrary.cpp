// Kakanu Portfolio Project


#include "Utils/PUBGFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "PUBGGameplayTags.h"
#include "System/PUBGAssetManager.h"
#include "PUBGTypes/PUBGStructTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UPUBGAbilitySystemComponent* UPUBGFunctionLibrary::NativeGetPUBGASCFromActor(AActor* InActor)
{
	check(InActor);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor);

	return (ASC) ? CastChecked<UPUBGAbilitySystemComponent>(ASC) : nullptr;
}

void UPUBGFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UPUBGAbilitySystemComponent* ASC = NativeGetPUBGASCFromActor(InActor);

	if (ASC && !ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UPUBGFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UPUBGAbilitySystemComponent* ASC = NativeGetPUBGASCFromActor(InActor);

	if (ASC && ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UPUBGFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UPUBGAbilitySystemComponent* ASC = NativeGetPUBGASCFromActor(InActor);
	
	return ASC && ASC->HasMatchingGameplayTag(TagToCheck);
}

void UPUBGFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EPUBGConfirmType& OutConfirmType)
{
	OutConfirmType = (NativeDoesActorHaveTag(InActor, TagToCheck) ? EPUBGConfirmType::Yes : EPUBGConfirmType::No);
}

void UPUBGFunctionLibrary::NativeSetMoveSpeed(ACharacter* InCharacter)
{
	// 무기 소지 여부 가져오기
	FString hasItem = NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_UseEquipItem)
		? TEXT("True") : TEXT("False");

	// 서있는 상태, 앉기, 엎드리기 중 하나 있는지 체크
	UCharacterMovementComponent* MovementComponent = InCharacter->GetCharacterMovement();
	float* MaxWalkSpeed = &MovementComponent->MaxWalkSpeed;

	FString State = TEXT("Stand");
	if (NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_Crouch))
	{
		State = TEXT("Crouch");
		MaxWalkSpeed = &MovementComponent->MaxWalkSpeedCrouched;
	}
	else if (NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_Prone))
	{
		State = TEXT("Prone");
	}

	FString WalkType = TEXT("Jog");
	if (NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_Sprint))
	{
		WalkType = TEXT("Sprint");
	}
	else if (NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_Walk))
	{
		WalkType = TEXT("Walk");
	}

	// 테이블에서 데이터 가져오기
	UDataTable* WalkSpeedDataTabel = UPUBGAssetManager::Get().GetAssetByName<UDataTable>("WalkSpeed");
	FString ContextString;
	/*FWalkSpeed* WalkSpeedStruct = WalkSpeedDataTabel->FindRow<FWalkSpeed>(FName(hasItem + TEXT("_") + State + TEXT("_") + WalkType), ContextString);
	
	check(WalkSpeedStruct);

	*MaxWalkSpeed = WalkSpeedStruct->Forward;*/
}

void UPUBGFunctionLibrary::NativeSetMoveSpeedByState(ACharacter* InCharacter, FString NextState)
{
	// 무기 소지 여부 가져오기
	FString hasItem = NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_UseEquipItem)
		? TEXT("True") : TEXT("False");

	// 서있는 상태, 앉기, 엎드리기 중 하나 있는지 체크
	UCharacterMovementComponent* MovementComponent = InCharacter->GetCharacterMovement();
	float* MaxWalkSpeed = &MovementComponent->MaxWalkSpeed;

	FString state = NextState;


	// 걷기 타입
	FString WalkType = TEXT("Jog");
	if (NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_Sprint))
	{
		state = TEXT("Sprint");
	}
	else if (NativeDoesActorHaveTag(InCharacter, PUBGGameplayTags::Status_Walk))
	{
		state = TEXT("Walk");
	}

	// 테이블에서 데이터 가져오기
	UDataTable* WalkSpeedDataTabel = UPUBGAssetManager::Get().GetAssetByName<UDataTable>("WalkSpeed");
	FString ContextString;
	//FWalkSpeed* WalkSpeedStruct = WalkSpeedDataTabel->FindRow<FWalkSpeed>(FName(hasItem + TEXT("_") + state + TEXT("_") + WalkType), ContextString);

	//check(WalkSpeedStruct);

	//*MaxWalkSpeed = WalkSpeedStruct->Forward;

}