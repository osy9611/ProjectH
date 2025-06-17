// Fill out your copyright notice in the Description page of Project Settings.


#include "HDAbilitySet.h"
#include "ProjectH/AbilitySystem/HDAbilitySystemComponent.h"
#include "ProjectH/AbilitySystem/Abilities/HDGameplayAbility.h"
#include "Animation/AnimMontage.h"
void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
		AbilitySpecHandles.Add(Handle);
}

void FAbilitySet_GrantedHandles::TakeFromAbilitySystem(UHDAbilitySystemComponent* ASC)
{
	if (!ASC->IsOwnerActorAuthoritative())
		return;

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
			ASC->ClearAbility(Handle);
	}
}

UHDAbilitySet::UHDAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHDAbilitySet::GiveToAbilitySystem(UHDAbilitySystemComponent* ASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
		return;

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
			continue;

		UHDGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UHDGameplayAbility>();

		if (AbilityToGrant.AnimMontage)
			AbilityCDO->AnimMontage = AbilityToGrant.AnimMontage;

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
	}
}
