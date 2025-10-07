// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGE_DeBuff.h"
#include "ProjectH/AbilitySystem/GameEffectComp/HDGEComp_DeBuff.h"
UHDGE_DeBuff::UHDGE_DeBuff()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

	UHDGEComp_DeBuff* GEComp = CreateDefaultSubobject<UHDGEComp_DeBuff>(TEXT("HDGEComp_DeBuff"));
	GEComponents.Add(GEComp);
}
