// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAbilitySystemComponent.h"

UHDAbilitySystemComponent::UHDAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UHDAbilitySystemComponent::ProcessAbilityAndParam(const FGameplayTag& Tag, const FGameAbilityParam& Params)
{
	if (Tag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && !AbilitySpec.IsActive() && (AbilitySpec.DynamicAbilityTags.HasTagExact(Tag)))
			{
				StoreParamsForAbility(AbilitySpec.Handle,Params);
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
void UHDAbilitySystemComponent::StoreParamsForAbility(FGameplayAbilitySpecHandle Handle, const FGameAbilityParam& Params)
{
	if (PendingParams.Contains(Handle))
	{
		PendingParams[Handle] = Params.Clone();
		return;
	}

	PendingParams.Add(Handle, Params.Clone());
}

FGameAbilityParam* UHDAbilitySystemComponent::ConsumeParams(FGameplayAbilitySpecHandle Handle)
{
	FGameAbilityParam** Params = PendingParams.Find(Handle);
	if (!Params)
		return nullptr;
	PendingParams.Remove(Handle);
	return *Params;
}
