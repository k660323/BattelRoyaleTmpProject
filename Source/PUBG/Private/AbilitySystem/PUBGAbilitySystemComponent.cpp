// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "PUBGGameplayTags.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "AbilitySystem/PUBGAbilityTagRelationshipMapping.h"
#include "System/PUBGAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PUBGAbilitySystemComponent)

UPUBGAbilitySystemComponent::UPUBGAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UPUBGAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// if (GetOwner()->HasAuthority())
	// {
	// 	GetGameplayAttributeValueChangeDelegate(
	// 		UPUBGInventorySet::GetAdditionalMaxCapacityWeightAttribute()
	// 	).Remove(Handle);
	// }

	Super::EndPlay(EndPlayReason);
}

void UPUBGAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();

	// 등록된 어빌리티 순회
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// 어빌리티가 활성화 중일 때만 처리
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		// PUBGGameplayAbility만 처리
		UPUBGGameplayAbility* PUBGAbilityCDO = Cast<UPUBGGameplayAbility>(AbilitySpec.Ability);
		if (!PUBGAbilityCDO)
		{
			UE_LOG(LogTemp, Error, TEXT("CancelAbilitiesByFunc: Non-PUBGGameplayAbility %s was Granted to ASC. Skipping."), *AbilitySpec.Ability.GetName());
			continue;
		}
	
		if (PUBGAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// Cancel all the spawned instances, not the CDO.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				UPUBGGameplayAbility* PUBGAbilityInstance = Cast<UPUBGGameplayAbility>(AbilityInstance);

				if (ShouldCancelFunc(PUBGAbilityInstance, AbilitySpec.Handle))
				{
					if (PUBGAbilityInstance->CanBeCanceled())
					{
						PUBGAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), PUBGAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("CancelAbilitiesByFunc : Can't cancel ability [%s] becacuse CanBeCanceled is false."), *PUBGAbilityInstance->GetName())
					}
				}
			}
		}
		else
		{
			// Cancel the non-instanced ability COD.
			if (ShouldCancelFunc(PUBGAbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled. 
				// Non-instanced 어빌리티는 모든 어빌리티가 공유 한다 그래서 변수를 가질수 없음
				PUBGAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
			}
		}
	}
}

void UPUBGAbilitySystemComponent::OnAbilityTagStarted(const FGameplayTag& InInputTag)
{
	if (InInputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)))
			{
				InputStartedSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UPUBGAbilitySystemComponent::OnAbilityTagPressed(const FGameplayTag& InInputTag)
{
	if (InInputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UPUBGAbilitySystemComponent::OnAbilityTagReleased(const FGameplayTag& InInputTag)
{
	if (InInputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UPUBGAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (HasMatchingGameplayTag(PUBGGameplayTags::TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//@TODO: See if we can use FScopedServerAbilityRPCBatcher ScopedRPCBatcher in some of these loops

	//
	// Process all abilities that activate when the input is held.
	// 홀딩은 이벤트 콜백을 보내지 않고 배열에 추가
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UPUBGGameplayAbility* PUBGAbilityCDO = Cast<UPUBGGameplayAbility>(AbilitySpec->Ability);
				if (PUBGAbilityCDO && PUBGAbilityCDO->GetActivationPolicy() == EPUBGAbilityActivationPolicy::OnHold)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//
	// Process all abilities that had their input started this frame.
	// 키입력시 이벤트 콜백 호출
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputStartedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				if (AbilitySpec->IsActive())
				{
					AbilitySecInputStarted(*AbilitySpec);
				}
			}
		}
	}

	//
	// Process all abilities that had their input pressed this frame.
	// 키를 누를때 이벤트 콜백 호출 및 배열에 추가
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UPUBGGameplayAbility* PUBGAbilityCDO = Cast<UPUBGGameplayAbility>(AbilitySpec->Ability);

					if (PUBGAbilityCDO && PUBGAbilityCDO->GetActivationPolicy() == EPUBGAbilityActivationPolicy::OnTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	// 홀딩이랑 프레스만 어빌리티 발동
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	//
	// Process all abilities that had their input released this frame.
	// 키입력이 풀릴때 이벤트만 보냄
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputStartedSpecHandles.Reset();
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UPUBGAbilitySystemComponent::ClearAbilityInput()
{
	InputStartedSpecHandles.Reset();
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

bool UPUBGAbilitySystemComponent::IsActivationGroupBlocked(EPUBGAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case EPUBGAbilityActivationGroup::Independent:
		// Independent abilities are never blocked.
		bBlocked = false;
		break;
	case EPUBGAbilityActivationGroup::Exclusive_Replaceable:
	case EPUBGAbilityActivationGroup::Exclusive_Blocking:
		// Exclusive abilities can activate if nothing is blocking.
		bBlocked = (ActivationGroupCounts[(uint8)EPUBGAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;
	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

void UPUBGAbilitySystemComponent::AddAbilityToActivationGroup(EPUBGAbilityActivationGroup Group, UPUBGGameplayAbility* PUBGAbility)
{
	check(PUBGAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	const bool bReplicateCancelAbility = false;

	switch (Group)
	{
	case EPUBGAbilityActivationGroup::Independent: // 독립적 (기본)
		// Independent abilities do not cancel any other abilities.
		break;
	case EPUBGAbilityActivationGroup::Exclusive_Replaceable:
	case EPUBGAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(EPUBGAbilityActivationGroup::Exclusive_Replaceable, PUBGAbility, bReplicateCancelAbility);
		break;
	default:
		checkf(false, TEXT("AddAbilityToActivationGroup : Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

}

void UPUBGAbilitySystemComponent::RemoveAbilityFromActivationGroup(EPUBGAbilityActivationGroup Group, UPUBGGameplayAbility* PUBGAbility)
{
	check(PUBGAbility);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

void UPUBGAbilitySystemComponent::CancelActivationGroupAbilities(EPUBGAbilityActivationGroup Group, UPUBGGameplayAbility* IgnorePUBGAbility, bool bReplicateCancelAbility)
{
	auto ShouldCancelFunc = [this, Group, IgnorePUBGAbility](const UPUBGGameplayAbility* PUBGAbility, FGameplayAbilitySpecHandle Handle)
		{
			return ((PUBGAbility->GetActivationGroup() == Group) && (PUBGAbility != IgnorePUBGAbility));
		};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UPUBGAbilitySystemComponent::AddDynamicTagGameplayEffect(FGameplayTag Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("Tag_AddDynamic"));
	check(DynamicTagGE);

	if (!DynamicTagGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect."));
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."), *GetNameSafe(DynamicTagGE));
		return;
	}

	Spec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*Spec);
}

FActiveGameplayEffectHandle UPUBGAbilitySystemComponent::AddDynamicTagsGameplayEffect(TArray<FGameplayTag> Tags)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("Tag_AddDynamic"));
	check(DynamicTagGE);

	if (!DynamicTagGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect."));
		return FActiveGameplayEffectHandle();
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."), *GetNameSafe(DynamicTagGE));
		return FActiveGameplayEffectHandle();
	}

	for (auto Tag : Tags)
	{
		Spec->DynamicGrantedTags.AddTag(Tag);
	}

	return ApplyGameplayEffectSpecToSelf(*Spec);
}

void UPUBGAbilitySystemComponent::RemoveDynamicTagGameplayEffect(FGameplayTag Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UPUBGAssetManager::GetSubclassByName<UGameplayEffect>(TEXT("Tag_AddDynamic"));

	if (!DynamicTagGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect."));
		return;
	}

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	Query.EffectDefinition = DynamicTagGE;

	RemoveActiveEffects(Query);
}

void UPUBGAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

void UPUBGAbilitySystemComponent::SetTagRelationshipMapping(UPUBGAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

//bool UPUBGAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
//{
//	check(AbilityTagToActivate.IsValid());
//
//	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
//	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), OUT FoundAbilitySpecs);
//
//	if (!FoundAbilitySpecs.IsEmpty())
//	{
//		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
//		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];
//
//		check(SpecToActivate);
//
//		if (!SpecToActivate->IsActive())
//		{
//			return TryActivateAbility(SpecToActivate->Handle);
//		}
//	}
//
//	return false;
//}

void UPUBGAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

bool UPUBGAbilitySystemComponent::HasChildTagOfParent(const FGameplayTag& InParentTag)
{
	const FGameplayTagContainer& OwnedTags = GetOwnedGameplayTags();
	for (const FGameplayTag& Tag : OwnedTags)
	{
		if (Tag.MatchesTag(InParentTag))
		{
			return true;
		}
	}

	return false;
}

//void UPUBGAbilitySystemComponent::HandleWeightChanged(float NewValue, const FGameplayEffectModCallbackData* Data)
//{
//	// Modifier 기반 처리
//	FPUBGAdditionalValueMessage Message;
//	Message.EventTag = PUBGGameplayTags::Message_AdditionalValue;
//	Message.ChangedAdditionalValue = NewValue;
//
//	// 브로드 캐스트
//	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
//	MessageSystem.BroadcastMessage(Message.EventTag, Message);
//}

void UPUBGAbilitySystemComponent::AbilitySecInputStarted(FGameplayAbilitySpec& Spec)
{
	if (Spec.IsActive())
	{
		TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
		if (Instances.Num() > 0)
		{
			const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
			FPredictionKey PredictionKey = ActivationInfo.GetActivationPredictionKey();

			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::GameCustom1, Spec.Handle, PredictionKey);
		}
	}
}

void UPUBGAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
		if (Instances.Num() > 0)
		{
			const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
			FPredictionKey PredictionKey = ActivationInfo.GetActivationPredictionKey();

			// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, PredictionKey);
		}
	}
}

void UPUBGAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
		if (Instances.Num() > 0)
		{
			const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
			FPredictionKey PredictionKey = ActivationInfo.GetActivationPredictionKey();

			// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, PredictionKey);
		}
	}
}

void UPUBGAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	if (UPUBGGameplayAbility* PUBGAbility = Cast<UPUBGGameplayAbility>(Ability))
	{
		AddAbilityToActivationGroup(PUBGAbility->GetActivationGroup(), PUBGAbility);
	}
}

void UPUBGAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	HandleAbilityFailed(Ability, FailureReason);
}

void UPUBGAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	if (UPUBGGameplayAbility* PUBGAbility = Cast<UPUBGGameplayAbility>(Ability))
	{
		RemoveAbilityFromActivationGroup(PUBGAbility->GetActivationGroup(), PUBGAbility);
	}
}

void UPUBGAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);
}

void UPUBGAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);

	//@TODO: Apply any special logic like blocking input or movement
}

void UPUBGAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void UPUBGAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	if (const UPUBGGameplayAbility* PUBGAbility = Cast<UPUBGGameplayAbility>(Ability))
	{
		PUBGAbility->OnAbilityFailedToActivate(FailureReason);
	}
}
