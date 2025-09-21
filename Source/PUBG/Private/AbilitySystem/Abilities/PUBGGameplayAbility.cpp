// Kakanu Portfolio Project


#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "AbilitySystemLog.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/PUBGBaseCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "GameFramework/Controller.h"
#include "Input/PUBGEnhancedPlayerInput.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystem/Abilities/PUBGAbilitySimpleFailureMessage.h"
#include "PUBGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGGameplayAbility)

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
{																																						\
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
		return ReturnValue;																																\
	}																																					\
}

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");


UPUBGGameplayAbility::UPUBGGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 복제X
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	// 각 액터로 관리
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// 로컬 예측 (로직 및 실행 타이밍)
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	// 보안정책 클라 또는 서버 (어빌리티 종료 조건)
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	
	ActivationPolicy = EPUBGAbilityActivationPolicy::OnTriggered;
	ActivationGroup = EPUBGAbilityActivationGroup::Independent;
}

void UPUBGGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	bool bSimpleFailureFound = false;
	for (FGameplayTag Reason : FailedReason)
	{
		if (!bSimpleFailureFound)
		{
			if (const FText* pUserFacingMessage = FailureTagToUserFacingMessages.Find(Reason))
			{
				FPUBGAbilitySimpleFailureMessage Message;
				Message.PlayerController = GetActorInfo().PlayerController.Get();
				Message.FailureTags = FailedReason;
				Message.UserFacingReason = *pUserFacingMessage;

				UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
				MessageSystem.BroadcastMessage(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, Message);
				bSimpleFailureFound = true;
			}
		}

		if (UAnimMontage* pMontage = FailureTagToAnimMontage.FindRef(Reason))
		{
			FPUBGAbilityMontageFailureMessage Message;
			Message.PlayerController = GetActorInfo().PlayerController.Get();
			Message.FailureTags = FailedReason;
			Message.FailureMontage = pMontage;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, Message);
		}
	}
}

bool UPUBGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UPUBGAbilitySystemComponent* PUBGASC = Cast<UPUBGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if (PUBGASC->IsActivationGroupBlocked(ActivationGroup))
	{
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(PUBGGameplayTags::Ability_ActivateFail_ActivationGroup);
		}

		return false;
	}

	return true;
}

void UPUBGGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	// The ability can not block canceling if it's replaceable.
	if (!bCanBeCanceled && (ActivationGroup == EPUBGAbilityActivationGroup::Exclusive_Replaceable))
	{
		UE_LOG(LogTemp, Error, TEXT("SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable."), *GetName());
		return;
	}

	Super::SetCanBeCanceled(bCanBeCanceled);
}

void UPUBGGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActivationPolicy == EPUBGAbilityActivationPolicy::OnGiven || ActivationPolicy == EPUBGAbilityActivationPolicy::OnSpawn)
	{
		const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

		// Try to activate if activation policy is on spawn.
		if (ActorInfo && !Spec.IsActive() && !bIsPredicting)
		{
			UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
			const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

			// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
			if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
			{
				const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
				const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

				const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
				const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

				if (bClientShouldActivate || bServerShouldActivate)
				{
					ASC->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UPUBGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo->IsNetAuthority() == false)
	{
		return;
	}

	if (ActivationPolicy == EPUBGAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

bool UPUBGGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	// Specialized version to handle death exclusion and AbilityTags expansion via ASC

	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}

	const UPUBGAbilitySystemComponent* PUBGASC = Cast<UPUBGAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// Expand our ability tags to add additional required/blocked tags
	if (PUBGASC)
	{
		PUBGASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}

	// Check to see the required/blocked tags for this ability
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;

		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(PUBGGameplayTags::Status_Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OptionalRelevantTags->AddTag(PUBGGameplayTags::Ability_ActivateFail_IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
}

UPUBGAbilitySystemComponent* UPUBGGameplayAbility::GetPUBGAbilitySystemComponentFromActorInfo() const
{
	return Cast<UPUBGAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

APUBGBaseCharacter* UPUBGGameplayAbility::GetBaseCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<APUBGBaseCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

AController* UPUBGGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		// 최상위 까지 가면서 컨트롤 액터 찾는다.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			// 해당 액터가 컨트롤러이면
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			// 해당 액터가 폰이면
			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			// 찾지못했으면 부모 액터로 변경
			TestActor = TestActor->GetOwner();
		}
	}
	return nullptr;
}

void UPUBGGameplayAbility::GetMovementDirection(EPUBGDirection& OutDirection, FVector& OutMovementVector)
{
	FVector FacingVector;

	if (APUBGBaseCharacter* BaseCharacter = GetBaseCharacterFromActorInfo())
	{
		FacingVector = BaseCharacter->GetActorForwardVector();

		if (UAIBlueprintHelperLibrary::GetAIController(BaseCharacter))
		{
			if (UNavigationSystemV1* NavigationSystemV1 = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				FNavLocation OutProjectedLocation;
				const FVector CharacterLocation = BaseCharacter->GetActorLocation();
				const FVector QueryExtent = FVector(500.f, 500.f, 500.f);
				NavigationSystemV1->ProjectPointToNavigation(CharacterLocation, OutProjectedLocation, QueryExtent);
				
				FVector CharacterToProjectedXY = (OutProjectedLocation.Location - CharacterLocation) * FVector(1.f, 1.f, 0.f);
				OutMovementVector = CharacterToProjectedXY.GetSafeNormal();
			}
		}
		else
		{
			OutMovementVector = BaseCharacter->GetLastMovementInputVector();
		}
	}

	const FRotator& FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(FacingVector);
	const FRotator& MovementRotator = UKismetMathLibrary::Conv_VectorToRotator(OutMovementVector);

	const FRotator& DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotator, FacingRotator);
	float YawAbs = FMath::Abs(DeltaRotator.Yaw);

	if (OutMovementVector.IsNearlyZero())
	{
		OutDirection = EPUBGDirection::None;
	}
	else
	{
		if (YawAbs < 60.f)
		{
			OutDirection = EPUBGDirection::Forward;
		}
		else if (YawAbs > 120.f)
		{
			OutDirection = EPUBGDirection::Backward;
		}
		else if (DeltaRotator.Yaw < 0.f)
		{
			OutDirection = EPUBGDirection::Left;
		}
		else
		{
			OutDirection = EPUBGDirection::Right;
		}
	}
}


FActiveGameplayEffectHandle UPUBGGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	check(TargetASC && InSpecHandle.IsValid());

	return GetPUBGAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle UPUBGGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EPUBGSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);

	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EPUBGSuccessType::Successful : EPUBGSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}

void UPUBGGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty())
	{
		return;
	}

	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

	for (const FHitResult& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			//if (UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;

					/*UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						PUBGGameplayTags::Shared_Event_HitReact,
						Data
					);*/
				}
			}
		}
	}
}

void UPUBGGameplayAbility::FlushPressedKey()
{
	if (APlayerController* PC = Cast<APlayerController>(GetControllerFromActorInfo()))
	{
		PC->FlushPressedKeys();
	}
}

void UPUBGGameplayAbility::FlushPressedInput(UInputAction* InputAction)
{
	if (CurrentActorInfo)
	{
		if (APlayerController* PlayerController = CurrentActorInfo->PlayerController.Get())
		{
			if (UPUBGEnhancedPlayerInput* EnhancedPlayerInput = Cast<UPUBGEnhancedPlayerInput>(PlayerController->PlayerInput))
			{
				EnhancedPlayerInput->FlushPressedInput(InputAction);
			}
		}
	}
}

EPUBGAbilityActivationPolicy UPUBGGameplayAbility::GetActivationPolicy() const
{
	return ActivationPolicy;
}

EPUBGAbilityActivationGroup UPUBGGameplayAbility::GetActivationGroup() const
{
	return ActivationGroup;
}

bool UPUBGGameplayAbility::CanChangeActivationGroup(EPUBGAbilityActivationGroup NewGroup) const
{
	if (!IsInstantiated() || !IsActive())
	{
		return false;
	}

	if (ActivationGroup == NewGroup)
	{
		return true;
	}

	UPUBGAbilitySystemComponent* PUBGASC = GetPUBGAbilitySystemComponentFromActorInfo();
	check(PUBGASC);

	if ((ActivationGroup != EPUBGAbilityActivationGroup::Exclusive_Blocking) && PUBGASC->IsActivationGroupBlocked(NewGroup))
	{
		// This ability can't change groups if it's blocked (unless it is the one doing the blocking).
		return false;
	}

	if ((NewGroup == EPUBGAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
	{
		// This ability can't become replaceable if it can't be canceled.
		return false;
	}

	return true;
}

bool UPUBGGameplayAbility::ChangeActivationGroup(EPUBGAbilityActivationGroup NewGroup)
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ChangeActivationGroup, false);

	if (!CanChangeActivationGroup(NewGroup))
	{
		return false;
	}

	if (ActivationGroup != NewGroup)
	{
		UPUBGAbilitySystemComponent* PUBGASC = GetPUBGAbilitySystemComponentFromActorInfo();
		check(PUBGASC);

		PUBGASC->RemoveAbilityFromActivationGroup(ActivationGroup, this);
		PUBGASC->AddAbilityToActivationGroup(NewGroup, this);

		ActivationGroup = NewGroup;
	}

	return true;
}
