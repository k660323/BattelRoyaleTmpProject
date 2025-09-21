// Kakanu Portfolio Project


#include "Data/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/PUBGAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/PUBGGameplayAbility.h"
#include "AttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DataAsset_StartUpDataBase)

// FPUBGStartUpData_GrantedHandles Begin
void FPUBGStartUpData_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& InHandle)
{
	if (InHandle.IsValid()) 
	{
		AbilitySpecHandles.Add(InHandle);
	}
}

void FPUBGStartUpData_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& InHandle)
{
	if (InHandle.IsValid())
	{
		EffectSpecHandles.Add(InHandle);
	}
}

void FPUBGStartUpData_GrantedHandles::AddAttributeSet(UAttributeSet* InAttributeSet)
{
	if (InAttributeSet)
	{
		GrantedAttributeSets.Add(InAttributeSet);
	}
}

void FPUBGStartUpData_GrantedHandles::ClearByAbilitySystem(UPUBGAbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			//ASC->CancelAbilityHandle(Handle);
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : EffectSpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* AttributeSet : GrantedAttributeSets)
	{
		if (AttributeSet)
		{
			ASC->RemoveSpawnedAttribute(AttributeSet);
		}
	}

	AbilitySpecHandles.Empty();
	EffectSpecHandles.Empty();
	GrantedAttributeSets.Empty();
}

// FPUBGStartUpData_GrantedHandles End

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UPUBGAbilitySystemComponent* InASCToGive, FPUBGStartUpData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(InASCToGive);

	// Ű�Է��� ���� Ư�� �����Ƽ-----
	// ��ȸ�� �����Ƽ �ڵ� �ߵ� (�����Ƽ ������ �����Ƽ ����)
	GrantAbilities(ActivateOnGiveAbilities, InASCToGive, OutGrantedHandles, SourceObject);
	// �̺�Ʈ �����Ƽ
	GrantAbilities(ReactiveAbilities, InASCToGive, OutGrantedHandles, SourceObject);
	// �����Ƽ �ڵ� �ߵ�  (�߰� Ű�Է¹��� �ʴ� ����)
	GrantAbilities(ActivateOnSpawnAbilities, InASCToGive, OutGrantedHandles, SourceObject);
	//-----


	// Ű�Է� �����Ƽ
	// �����Ƽ �ڵ� �ߵ� (�߰� Ű�Է� ������ �ְ� ���� ������)
	GrantAbilities(ActivateOnSpawnAbilitySets, InASCToGive, OutGrantedHandles, SourceObject);
	// Ű�Է� �����Ƽ
	GrantAbilities(StartAbilitySets, InASCToGive, OutGrantedHandles, SourceObject);
	//-----
	
	// ���� ����Ʈ
	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const FPUBGGameplayEffectSet& EffectSet : StartUpGameplayEffects)
		{
			if (!EffectSet.IsValid())
			{
				continue;
			}

			const TSubclassOf<UGameplayEffect>& Effect = EffectSet.GameplayEffect;
			UGameplayEffect* EffectCDO = Effect->GetDefaultObject<UGameplayEffect>();

			const FActiveGameplayEffectHandle EffectHandle = InASCToGive->ApplyGameplayEffectToSelf(EffectCDO, EffectSet.EffectLevel, InASCToGive->MakeEffectContext());
		
			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddGameplayEffectHandle(EffectHandle);
			}
		}
	}


	// ���� ��Ʈ����Ʈ��
	if (!StartUpAttributes.IsEmpty())
	{
		for (const FPUBGAttributeSetSet& AttributeSet : StartUpAttributes)
		{
			if (!AttributeSet.IsValid())
			{
				continue;
			}

			UAttributeSet* NewSet = NewObject<UAttributeSet>(InASCToGive->GetOwner(), AttributeSet.AttributeSet);
			InASCToGive->AddAttributeSetSubobject(NewSet);

			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddAttributeSet(NewSet);
			}
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<FPUBGAbilitySet>& InAbilitiesToGive, UPUBGAbilitySystemComponent* InASCToGive, FPUBGStartUpData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const FPUBGAbilitySet& AbilitySet : InAbilitiesToGive)
	{
		if (!AbilitySet.IsValid())
		{
			continue;
		}

		const TSubclassOf<UPUBGGameplayAbility>& Ability = AbilitySet.AbilityToGrant;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.Level = AbilitySet.AbilityLevel;

		const FGameplayAbilitySpecHandle AbilitySepcHandle = InASCToGive->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySepcHandle);
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<FPUBGGameplayTagAbilitySet>& InAbilitiesToGive, UPUBGAbilitySystemComponent* InASCToGive, FPUBGStartUpData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	for (const FPUBGGameplayTagAbilitySet& AbilitySet : InAbilitiesToGive)
	{
		if (!AbilitySet.IsValid())
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.Level = AbilitySet.AbilityLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = InASCToGive->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}