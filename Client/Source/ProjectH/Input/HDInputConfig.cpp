// Fill out your copyright notice in the Description page of Project Settings.


#include "HDInputConfig.h"
#include "ProjectH/LogChannels.h"

UHDInputConfig::UHDInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UInputAction* UHDInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FHDInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
			return Action.InputAction;
	}

	if (bLogNotFound)
		UE_LOG(HDLog, Error, TEXT("can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));

	return nullptr;
}

const UInputAction* UHDInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FHDInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
			return Action.InputAction;
	}

	if (bLogNotFound)
		UE_LOG(HDLog, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	return nullptr;
}

const UInputAction* UHDInputConfig::FindNativeInputActionForTag(const FString& InputTag, bool bLogNotFound) const
{
	if (InputTag.IsEmpty())
		return nullptr;

	FName TagName = FName(InputTag);
	FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(TagName);

	return FindNativeInputActionForTag(GameplayTag, bLogNotFound);
}
